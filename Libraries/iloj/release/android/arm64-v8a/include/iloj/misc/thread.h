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

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <string>

#ifdef _WIN64
    #include <windows.h>
    #include<processthreadsapi.h>
#endif
namespace iloj::misc
{
//! \brief SpinLock class implementing a light pulling-based mutex.
class SpinLock
{
private:
    std::atomic_flag m_locker{};

public:
    //! \brief Constructor.
    SpinLock() { m_locker.clear(); }
    SpinLock(const SpinLock &) = delete;
    SpinLock(SpinLock &&) = delete;
    //! \brief Destructor.
    ~SpinLock() = default;
    auto operator=(const SpinLock &) -> SpinLock & = delete;
    auto operator=(SpinLock &&) -> SpinLock & = delete;
    //! \brief Tries to lock the spinlock and return true if successfull (non-blocking).
    auto try_lock() -> bool { return !m_locker.test_and_set(std::memory_order_acquire); }
    //! \brief Locks the spinlock (blocking).
    void lock()
    {
        while (m_locker.test_and_set(std::memory_order_acquire))
        {
        }
    }
    //! \brief Unlocks the spinlock.
    void unlock() { m_locker.clear(std::memory_order_release); }
};

//! \brief Thread-safe variable with parametrizable mutex-type.
template<typename T, typename MUTEX = SpinLock>
class pVar
{
private:
    mutable MUTEX m_access{};
    T m_data{};

public:
    //! \brief Constructor.
    pVar() = default;
    pVar(const pVar &) = delete;
    pVar(pVar &&) = delete;
    //! \brief Destructor.
    ~pVar() = default;
    auto operator=(const pVar &) -> pVar & = delete;
    auto operator=(pVar &&) -> pVar & = delete;
    //! \brief Value based constructor.
    explicit pVar(const T &t): m_data(t) {}
    //! \brief Thread-safe value writing.
    void set(const T &t)
    {
        std::lock_guard<MUTEX> locker(m_access);
        m_data = t;
    }
    //! \brief Thread-safe value reading.
    auto get() const -> T
    {
        std::lock_guard<MUTEX> locker(m_access);
        return m_data;
    }
    //! \brief Locks the underlying mutex (blocking).
    void lock() { m_access.lock(); }
    //! \brief Tries to lock the underlying mutex and return true if successfull (non-blocking).
    auto try_lock() -> bool { return m_access.try_lock(); }
    //! \brief Unlock sthe underlying mutex.
    void unlock() { m_access.unlock(); }
    //! \brief Non thread-safe value writing (should be locked before).
    void unsafe_set(const T &t) { m_data = t; }
    auto unsafe_get() -> T & { return m_data; }
    //! \brief Non thread-safe value reading (should be locked before).
    auto unsafe_get() const -> const T & { return m_data; }
    //! \brief Non thread-safe access to value pointer
    auto operator->() -> T * { return &(m_data); }
    auto operator->() const -> const T * { return &(m_data); }
};

//! \brief Semaphore implementation.
class Semaphore
{
private:
    int m_value{};
    int m_wakeups{};
    mutable std::mutex m_mutex{};
    std::condition_variable m_cond{};

public:
    //! \brief Constructor.
    explicit Semaphore(int value = 0);
    Semaphore(const Semaphore &) = delete;
    Semaphore(Semaphore &&) = delete;
    //! \brief Destructor.
    ~Semaphore() = default;
    auto operator=(const Semaphore &) -> Semaphore & = delete;
    auto operator=(Semaphore &&) -> Semaphore & = delete;
    //! \brief Resets the number of pending ressources.
    void reset(int value = 0);
    //! \brief Waits for an available ressources.
    void wait();
    //! \brief Waits for an available ressources for ms and returns true if successfull.
    auto wait_for(std::chrono::milliseconds ms) -> bool;
    //! \brief Waits for n available ressources.
    void wait(int n);
    //! \brief Signals a new available ressources to all pending processes.
    void signal();
    //! \brief Signals a new available ressources to one unique pending process.
    void unique_signal();
    //! \brief Signals n new available ressources to all pending processes.
    void signal(int n);
    //! \brief Return sthe number of available ressources.
    auto available() const -> int;
};

//! \brief Service class implementing a pattern of interactive threaded process.
class Service
{
public:
    enum class StateId
    {
        Starting,
        Running,
        Finished,
        Stopping,
        Joining,
        Stopped
    };

private:
    pVar<StateId, SpinLock> m_state;
    std::atomic<bool> m_flag;
    std::unique_ptr<std::thread> m_thread;
    std::wstring m_serviceName = L"unnamed";

public:
    //! \brief Constructor.
    Service();
    Service(const Service &) = delete;
    Service(Service &&) = delete;
    //! \brief Destructor.
    virtual ~Service() = default;
    auto operator=(const Service &) -> Service & = delete;
    auto operator=(Service &&) -> Service & = delete;
    //! \brief Get service current state.
    auto state() const -> StateId { return m_state.get(); }
    //! \brief Starts the service.
    void start();
    //! \brief Stops the service.
    void stop();
    //! \brief Joins the service.
    void join();
    //! \brief True if the service is running.
    auto running() const -> bool { return m_flag; }
    //! \brief Ask the service to finish.
    void finish() { m_flag = false; }

    void setServiceName(std::wstring name) { m_serviceName = name;}

private:
    void run();
    static void execute(Service *s);

protected:
    //! \brief Virtual function to overload executed before the service is started.
    virtual void onStart() {}
    //! \brief Virtual function to overload executed after the service is stopped.
    virtual void onStop() {}
    //! \brief Virtual function to overload executed once before idle loop.
    virtual void initialize() {}
    //! \brief Virtual function to overload implementing the main task of the service.
    virtual void idle() {}
    //! \brief Virtual function to overload executed once after idle loop.
    virtual void finalize() {}
};

//! \brief Parallel for implementation splitting nbIter iterations of fun(id) on nbThread.
void parallel_for(std::size_t nbIter,
                  std::function<void(std::size_t)> fun,
                  unsigned nbThread = std::thread::hardware_concurrency());

//! \brief Parallel for implementation splitting iterations of fun(x, y) on nbThread over a 2D array.
void parallel_for(std::size_t w,
                  std::size_t h,
                  std::function<void(std::size_t, std::size_t)> fun,
                  unsigned nbThread = std::thread::hardware_concurrency());
} // namespace iloj::misc
