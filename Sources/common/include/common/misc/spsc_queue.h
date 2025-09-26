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
#include <vector>

namespace common::misc
{
template<typename T>
class SpScQueue
{
    // This implementation of the queue relies on a ring buffer mechanism for which the states of
    // emptiness and fullness are obtained from the positions of the head and the tail. By design,
    // the actual capacity of the ring will be one less than the size of the vector containing the
    // elements of the ring, meaning that one memory element of the vector is wasted. This is a
    // tradeoff to allow the lock-free implementation of the queue.
    // The size argument at construction and at the resize method should be understood as the
    // desired number of useful elements in the queue.

    static constexpr size_t defaultSize{4};

    std::vector<T> m_buffer;
    size_t m_size{defaultSize};
    std::atomic_size_t m_head{0};
    std::atomic_size_t m_tail{0};

public:
    SpScQueue(): m_buffer(defaultSize) {}
    SpScQueue(size_t size): m_buffer(size+1), m_size{size+1} {}

    void resize(size_t size)
    {
        m_buffer.resize(size+1);
        m_size = size+1;
    }

    void push(T &&t)
    {
        m_buffer[m_tail] = std::move(t);
        size_t next = (m_tail + 1) % m_size;
        m_tail = next;
    }

    void copy_push(T &t)
    {
        m_buffer[m_tail] = t;
        size_t next = (m_tail + 1) % m_size;
        m_tail = next;
    }

    T &front() { return m_buffer[m_head]; }

    void pop()
    {
        size_t next = (m_head + 1) % m_size;
        m_head = next;
    }

    template<typename Lambda>
    void pop(Lambda &onPop)
    {
        size_t head = m_head;
        onPop(m_buffer[head]);
        size_t next = (head + 1) % m_size;
        m_head = next;
    }

    bool empty() { return m_head == m_tail; }

    bool full() { return (m_tail + 1) % m_size == m_head; } //(m_size + m_head - m_tail - 1) % m_size == 0; }

    // the following need to be specialized for objects holding memory
    // and are not thread safe
    void clear()
    {
        m_head = 0;
        m_tail = 0;
    }

    template<typename Lambda>
    void iterate(Lambda &action)
    {
        for (T &t : m_buffer)
            action(t);
    }
};

} // namespace common::misc
