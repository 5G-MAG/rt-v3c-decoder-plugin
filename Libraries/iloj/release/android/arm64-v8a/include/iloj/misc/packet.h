/*
* Copyright (c) 2025 InterDigital CE Patent Holdings SASU
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

#pragma once

#include "thread.h"
#include <algorithm>
#include <queue>

namespace iloj::misc
{
template<typename T>
class Packet
{
private:
    struct Context
    {
    public:
        SpinLock m_access;
        T m_data;
        unsigned m_useCount;
        std::function<void()> m_onUniqueness;

    public:
        explicit Context(T data): m_data(std::move(data)), m_useCount(1) {}
    };

private:
    mutable SpinLock m_access;
    Context *m_context = nullptr;

public:
    Packet() = default;
    explicit Packet(T data): m_context(new Context(std::move(data))) {}
    Packet(const Packet &p)
    {
        std::lock_guard<SpinLock> locker_packet(p.m_access);

        if (p.m_context)
        {
            std::lock_guard<SpinLock> locker_context(p.m_context->m_access);

            m_context = p.m_context;
            m_context->m_useCount++;
        }
    }
    Packet(Packet &&p) noexcept
    {
        std::lock_guard<SpinLock> locker_packet(p.m_access);

        if (p.m_context)
        {
            std::lock_guard<SpinLock> locker_context(p.m_context->m_access);

            m_context = p.m_context;
            p.m_context = nullptr;
        }
    }
    // NOLINTNEXTLINE
    auto operator=(const Packet &p) -> Packet &
    {
        if (this != &p)
        {
            std::lock_guard<SpinLock> locker_packet_src(m_access);
            std::lock_guard<SpinLock> locker_packet_target(p.m_access);

            removeReference();

            if (p.m_context)
            {
                std::lock_guard<SpinLock> locker_context(p.m_context->m_access);

                m_context = p.m_context;

                if (m_context)
                {
                    m_context->m_useCount++;
                }
            }
        }

        return *this;
    }
    auto operator=(Packet &&p) noexcept -> Packet &
    {
        std::lock_guard<SpinLock> locker_packet_src(m_access);
        std::lock_guard<SpinLock> locker_packet_target(p.m_access);

        removeReference();

        if (p.m_context)
        {
            std::lock_guard<SpinLock> locker_context(p.m_context->m_access);

            m_context = p.m_context;
            p.m_context = nullptr;
        }

        return *this;
    }
    ~Packet() { reset(); }
    explicit operator bool() const
    {
        std::lock_guard<SpinLock> locker_packet(m_access);
        return (m_context != nullptr);
    }
    void setOnUniquenessCallback(const std::function<void()> &onUniqueness)
    {
        std::lock_guard<SpinLock> locker_packet(m_access);

        if (m_context)
        {
            std::lock_guard<SpinLock> locker(m_context->m_access);
            m_context->m_onUniqueness = onUniqueness;
        }
    }
    auto operator->() -> T * { return &(m_context->m_data); }
    auto operator->() const -> const T * { return &(m_context->m_data); }
    auto getContent() -> T & { return m_context->m_data; }
    auto getContent() const -> const T & { return m_context->m_data; }
    void reset()
    {
        std::lock_guard<SpinLock> locker_packet(m_access);

        removeReference();
    }

protected:
    void removeReference()
    {
        if (m_context)
        {
            m_context->m_access.lock();

            m_context->m_useCount--;

            if ((m_context->m_useCount == 1) && m_context->m_onUniqueness)
            {
                m_context->m_onUniqueness();
                m_context->m_access.unlock();
            }
            else if (!m_context->m_useCount)
            {
                m_context->m_access.unlock();
                delete m_context;
            }
            else
            {
                m_context->m_access.unlock();
            }

            m_context = nullptr;
        }
    }
};

template<typename T, typename... Args>
auto make_packet(Args &&... args) -> Packet<T>
{
    return Packet<T>(T{std::forward<Args>(args)...});
}

template<typename T>
class Factory
{
public:
    using iterator = typename std::vector<Packet<T>>::iterator;

private:
    SpinLock m_access;
    std::vector<Packet<T>> m_objects;
    Semaphore m_sAvailable;
    std::queue<unsigned> m_available;

public:
    explicit Factory(unsigned n): m_sAvailable(static_cast<int>(n))
    {
        m_objects.reserve(n);

        for (unsigned i = 0; i < n; i++)
        {
            Packet<T> p = make_packet<T>();

            p.setOnUniquenessCallback([this, i]() { release(i); });

            m_objects.push_back(std::move(p));
            m_available.push(i);
        }
    }
    Factory(const Factory &) = delete;
    Factory(Factory &&) = delete;
    ~Factory() { m_sAvailable.wait(m_objects.size()); }
    auto operator=(const Factory &) -> Factory & = delete;
    auto operator=(Factory &&) -> Factory & = delete;
    auto acquire() -> Packet<T>
    {
        m_sAvailable.wait();

        {
            std::lock_guard<SpinLock> locker(m_access);

            unsigned id = m_available.front();
            m_available.pop();

            return m_objects[id];
        }
    }
    auto full() -> bool
    {
        std::lock_guard<SpinLock> locker(m_access);
        return (m_objects.size() == m_available.size());
    }
    auto size() const -> std::size_t { return m_objects.size(); }
    auto begin() -> iterator { return m_objects.begin(); }
    auto end() -> iterator { return m_objects.end(); }

protected:
    void release(unsigned id)
    {
        {
            std::lock_guard<SpinLock> locker(m_access);
            m_available.push(id);
        }

        m_sAvailable.signal();
    }
};

template<typename T>
class Input
{
public:
    using iterator = typename std::deque<Packet<T>>::iterator;

private:
    SpinLock m_access;
    std::atomic<bool> m_flag{};
    std::deque<Packet<T>> m_pendingData{};
    Semaphore m_sIn;

public:
    Input() { m_flag = false; }
    auto is_open() -> bool
    {
        std::lock_guard<SpinLock> lock(m_access);
        return m_flag;
    }
    void open()
    {
        std::lock_guard<SpinLock> lock(m_access);
        if (!m_flag)
        {
            m_flag = true;
            m_sIn.reset();
        }
    }
    void close()
    {
        std::lock_guard<SpinLock> lock(m_access);
        if (m_flag)
        {
            m_flag = false;
            m_sIn.signal();
        }
    }
    void clear()
    {
        std::lock_guard<SpinLock> lock(m_access);
        m_pendingData.clear();
        m_sIn.reset();
    }
    auto pending() -> unsigned
    {
        std::lock_guard<SpinLock> lock(m_access);
        return m_pendingData.size();
    }
    auto empty() -> bool { return (pending() == 0U); }
    auto wait() -> bool
    {
        if (m_flag)
        {
            m_sIn.wait();
            m_sIn.signal();
        }
        return m_flag;
    }
    auto wait_for(std::chrono::milliseconds ms) -> bool
    {
        bool b = false;
        if (m_flag)
        {
            b = m_sIn.wait_for(ms);
            if (b)
            {
                m_sIn.signal();
            }
        }
        return (b && m_flag);
    }
    auto bufferize(int n) -> bool
    {
        if (m_flag)
        {
            int i = 0;
            for (; i < n; i++)
            {
                m_sIn.wait();
                if (!m_flag)
                {
                    break;
                }
            }
            m_sIn.signal(i);
        }
        return m_flag;
    }
    auto front() -> Packet<T> &
    {
        std::lock_guard<SpinLock> lock(m_access);
        return m_pendingData.front();
    }
    auto back() -> Packet<T> &
    {
        std::lock_guard<SpinLock> lock(m_access);
        int n = m_pendingData.size() - 1;

        for (int i = 0; i < n; i++)
        {
            m_sIn.wait();
            m_pendingData.pop_front();
        }

        return m_pendingData.front();
    }
    void pop()
    {
        if (m_flag)
        {
            std::lock_guard<SpinLock> lock(m_access);

            if (!m_pendingData.empty())
            {
                m_sIn.wait();
                m_pendingData.pop_front();
            }
        }
    }
    void pop_while(const std::function<bool(const T &)> &condition)
    {
        if (m_flag)
        {
            std::lock_guard<SpinLock> lock(m_access);

            while (!m_pendingData.empty() && condition(m_pendingData.front().getContent()))
            {
                m_sIn.wait();
                m_pendingData.pop_front();
            }
        }
    }
    void push(const Packet<T> &p)
    {
        std::lock_guard<SpinLock> lock(m_access);

        if (m_flag)
        {
            m_pendingData.push_back(p);
            m_sIn.signal();
        }
    }

    template<typename Pred>
    void insert(const Packet<T> &p, Pred pred)
    {
        std::lock_guard<SpinLock> lock(m_access);

        if (m_flag)
        {
            m_pendingData.insert(std::upper_bound(m_pendingData.begin(), m_pendingData.end(), p, pred), p);
            m_sIn.signal();
        }
    }
    auto begin() -> iterator { return m_pendingData.begin(); }
    auto end() -> iterator { return m_pendingData.end(); }
    auto size() { return m_pendingData.size(); }
};

template<typename T>
class Output
{
private:
    SpinLock m_access;
    std::vector<Input<T> *> m_inputs{};

public:
    void addInput(Input<T> &input)
    {
        std::lock_guard<SpinLock> lock(m_access);
        m_inputs.push_back(&input);
    }
    void removeInput(Input<T> &input)
    {
        std::lock_guard<SpinLock> lock(m_access);
        m_inputs.erase(std::find(m_inputs.begin(), m_inputs.end(), &input));
    }
    void push(const Packet<T> &p)
    {
        std::lock_guard<SpinLock> lock(m_access);
        for (auto &c : m_inputs)
        {
            c->push(p);
        }
    }
};

template<typename T>
void connect(Output<T> &out, Input<T> &in)
{
    out.addInput(in);
}

template<typename T>
void disconnect(Output<T> &out, Input<T> &in)
{
    out.removeInput(in);
}
} // namespace iloj::misc
