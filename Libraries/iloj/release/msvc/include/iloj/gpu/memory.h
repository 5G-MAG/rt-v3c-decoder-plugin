/*
* Copyright (c) 2024 InterDigital R&D France
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

#pragma once

#include <queue>

namespace iloj::gpu
{
namespace Object
{
class Base
{
    friend class Manager;

private:
    int m_objectId = -1;

public:
    Base();
    virtual ~Base();
    Base(const Base &) = default;
    Base(Base &&) = default;
    auto operator=(const Base &) -> Base & = default;
    auto operator=(Base &&) -> Base & = default;

protected:
    void unregister();
    virtual void destroy() = 0;
};

class Manager
{
    friend class Base;

private:
    std::queue<int> m_available;
    std::vector<Base *> m_objects;

public:
    Manager() = default;
    ~Manager() = default;
    Manager(const Manager &) = default;
    Manager(Manager &&) = default;
    auto operator=(const Manager &) -> Manager & = default;
    auto operator=(Manager &&) -> Manager & = default;
    void clear();

private:
    auto doRegister(Base *pObj) -> int;
    bool doUnregister(Base *pObj);
};
} // namespace Object
} // namespace iloj::gpu
