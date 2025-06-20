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

#include "logger.h"
#include <iosfwd>
#include <limits>
#include <map>
#include <memory>
#include <sstream>
#include <vector>

namespace iloj::misc
{
namespace JSON
{
enum class Type
{
    Integer,
    Float,
    String,
    Boolean,
    Array,
    Object,
    Null,
    Invalid
};

class Item
{
protected:
    Type m_type;
    std::string m_newline = "\n", m_indent = "\t", m_termination = "\n";

public:
    explicit Item(Type type = Type::Invalid): m_type(type) {}
    virtual ~Item() = default;
    Item(const Item &) = default;
    Item(Item &&) noexcept = default;
    auto operator=(const Item &) -> Item & = default;
    auto operator=(Item &&) noexcept -> Item & = default;
    [[nodiscard]] auto getType() const -> Type { return m_type; }
    void setSeparators(std::string newline, std::string indent, std::string termination)
    {
        m_newline = std::move(newline);
        m_indent = std::move(indent);
        m_termination = std::move(termination);
    }
    virtual auto flow(std::ostream &os, unsigned level) const -> std::ostream &
    {
        return flow(os, m_newline, m_indent, m_termination, level);
    }
    virtual auto flow(std::ostream &os,
                      const std::string & /*newline*/,
                      const std::string & /*indent*/,
                      const std::string & /*termination*/,
                      unsigned /*level*/) const -> std::ostream &
    {
        return os;
    }
    [[nodiscard]] auto str() const -> std::string;
    [[nodiscard]] auto str(const std::string &newline, const std::string &indent, const std::string &termination) const
        -> std::string;
    template<typename T>
    auto as() const -> T;
    template<typename T>
    auto asVectorOf() const -> std::vector<T>;
    template<typename T>
    auto castTo() -> T &
    {
        return dynamic_cast<T &>(*this);
    }
    template<typename T>
    [[nodiscard]] [[nodiscard]] [[nodiscard]] [[nodiscard]] [[nodiscard]] [[nodiscard]] [[nodiscard]] auto
    castTo() const -> const T &
    {
        return dynamic_cast<const T &>(*this);
    }
    virtual explicit operator bool() const { return false; }
    template<typename T>
    static auto getType() -> Type;
};

template<>
[[nodiscard]] auto Item::as() const -> int;
template<>
[[nodiscard]] auto Item::as() const -> unsigned;
template<>
[[nodiscard]] auto Item::as() const -> float;
template<>
[[nodiscard]] auto Item::as() const -> double;
template<>
[[nodiscard]] auto Item::as() const -> std::string;
template<>
[[nodiscard]] auto Item::as() const -> bool;

template<>
auto Item::getType<int>() -> Type;
template<>
auto Item::getType<unsigned>() -> Type;
template<>
auto Item::getType<float>() -> Type;
template<>
auto Item::getType<double>() -> Type;
template<>
auto Item::getType<std::string>() -> Type;
template<>
auto Item::getType<bool>() -> Type;

auto make_item(const std::string &input, std::size_t beg, std::size_t end) -> std::unique_ptr<Item>;

class Null: public Item
{
public:
    Null(): Item(Type::Null) {}
    ~Null() override = default;
    Null(const Null &) = default;
    Null(Null &&) noexcept = default;
    auto operator=(const Null &) -> Null & = default;
    auto operator=(Null &&) noexcept -> Null & = default;
    auto flow(std::ostream &os,
              const std::string & /*unused*/,
              const std::string & /*unused*/,
              const std::string & /*unused*/,
              unsigned /*unused*/) const -> std::ostream & override
    {
        os << "null";
        return os;
    }
    explicit operator bool() const override { return false; }
};

template<typename T>
class Regular: public Item
{
private:
    T m_value{};

public:
    Regular(): Item(Item::getType<T>()) {}
    ~Regular() override = default;
    Regular(const Regular &) = default;
    Regular(Regular &&) noexcept = default;
    auto operator=(const Regular &) -> Regular & = default;
    auto operator=(Regular &&) noexcept -> Regular & = default;
    explicit Regular(T value): Item(Item::getType<T>()), m_value(std::move(value)) {}
    template<typename U>
    explicit Regular(const Regular<U> &other): m_value{other.m_value}
    {
    }
    [[nodiscard]] auto getValue() const -> const T & { return m_value; }
    void setValue(T v) { m_value = std::move(v); }
    auto flow(std::ostream &os,
              const std::string &newline,
              const std::string &indent,
              const std::string &termination,
              unsigned level) const -> std::ostream & override;
    explicit operator bool() const override { return true; }
};

using Boolean = Regular<bool>;
using Integer = Regular<int>;
using Float = Regular<double>;
using String = Regular<std::string>;

template<>
auto Regular<bool>::flow(std::ostream &os,
                         const std::string & /*unused*/,
                         const std::string & /*unused*/,
                         const std::string & /*unused*/,
                         unsigned /*unused*/) const -> std::ostream &;
template<>
auto Regular<int>::flow(std::ostream &os,
                        const std::string & /*unused*/,
                        const std::string & /*unused*/,
                        const std::string & /*unused*/,
                        unsigned /*unused*/) const -> std::ostream &;
template<>
auto Regular<double>::flow(std::ostream &os,
                           const std::string & /*unused*/,
                           const std::string & /*unused*/,
                           const std::string & /*unused*/,
                           unsigned /*unused*/) const -> std::ostream &;
template<>
auto Regular<std::string>::flow(std::ostream &os,
                                const std::string & /*unused*/,
                                const std::string & /*unused*/,
                                const std::string & /*unused*/,
                                unsigned /*unused*/) const -> std::ostream &;

class Object: public Item
{
private:
    mutable std::map<std::string, std::unique_ptr<Item>> m_attribute;

public:
    Object(): Item(Type::Object) {}
    ~Object() override = default;
    Object(const Object &other): Object(other.str()) {}
    Object(Object &&) noexcept = default;
    auto operator=(const Object &other) -> Object &
    {
        *this = Object(other.str());
        return *this;
    }
    auto operator=(Object &&) noexcept -> Object & = default;
    explicit Object(const std::string &input,
                    std::size_t beg = 0,
                    std::size_t end = (std::numeric_limits<std::size_t>::max)());
    auto isEmpty() const -> bool { return m_attribute.empty(); }
    auto hasItem(const std::string &name) const -> bool { return (m_attribute.find(name) != m_attribute.end()); }
    auto setItem(const std::string &name, std::unique_ptr<Item> item) -> Item &
    {
        return *(m_attribute[name] = std::move(item));
    }
    template<typename T, typename... Args>
    auto setItem(const std::string &name, Args &&... args) -> T &
    {
        return dynamic_cast<T &>(*(m_attribute[name] = std::make_unique<T>(std::forward<Args>(args)...)));
    }
    auto getType(const std::string &name) const -> Type { return m_attribute[name]->getType(); }
    template<typename T>
    auto getItem(const std::string &name) -> T &
    {
        auto iter = m_attribute.find(name);

        if (iter != m_attribute.end())
        {
            return iter->second->castTo<T>();
        }
        {
            return setItem<T>(name);
        }
    }
    template<typename T = Item>
    auto getItem(const std::string &name) const -> const T &
    {
        static const T defaultObject;
        auto iter = m_attribute.find(name);

        if (iter != m_attribute.end())
        {
            return iter->second->castTo<T>();
        }

        LOG_WARNING("Key ", '\"', name, '\"', " not found");
        return defaultObject;
    }
    auto getItems() -> std::map<std::string, std::unique_ptr<Item>> & { return m_attribute; }
    auto getItems() const -> const std::map<std::string, std::unique_ptr<Item>> & { return m_attribute; }
    auto flow(std::ostream &os, unsigned level = 0) const -> std::ostream & override;
    auto flow(std::ostream &os,
              const std::string &newline,
              const std::string &indent,
              const std::string &termination,
              unsigned level = 0) const -> std::ostream & override;
    void clear() { m_attribute.clear(); }
    static auto fromFile(const std::string &path) -> Object;
    void toFile(const std::string &path) const;
    explicit operator bool() const override { return true; }
};

class Array: public Item
{
private:
    std::vector<std::unique_ptr<Item>> m_element;

public:
    Array(): Item(Type::Array) {}
    ~Array() override = default;
    Array(const Array &other): Array(other.str()) {}
    Array(Array &&) noexcept = default;
    auto operator=(const Array &other) -> Array &
    {
        *this = Array(other.str());
        return *this;
    }
    auto operator=(Array &&) noexcept -> Array & = default;
    Array(const std::string &input, std::size_t beg = 0, std::size_t end = (std::numeric_limits<std::size_t>::max)());
    [[nodiscard]] auto getSize() const -> std::size_t { return m_element.size(); }
    auto addItem(std::unique_ptr<Item> item) -> Item &
    {
        m_element.push_back(std::move(item));
        return *m_element.back();
    }
    template<typename T, typename... Args>
    auto addItem(Args &&... args) -> T &
    {
        m_element.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        return dynamic_cast<T &>(*m_element.back());
    }
    auto setItem(unsigned i, std::unique_ptr<Item> item) -> Item &
    {
        if (i < m_element.size())
        {
            return *(m_element[i] = std::move(item));
        }

        while ((m_element.size() + 1) < i)
        {
            m_element.push_back(std::make_unique<Item>());
        }

        return addItem(std::move(item));
    }
    template<typename T, typename... Args>
    auto setItem(unsigned i, Args &&... args) -> T &
    {
        if (i < m_element.size())
        {
            return dynamic_cast<T &>(*(m_element[i] = std::make_unique<T>(std::forward<Args>(args)...)));
        }

        while ((m_element.size() + 1) < i)
        {
            m_element.push_back(std::make_unique<Item>());
        }

        return addItem<T>(std::forward<Args>(args)...);
    }
    [[nodiscard]] auto getType(unsigned i) const -> Type { return m_element[i]->getType(); }
    template<typename T>
    auto getItem(unsigned i) -> T &
    {
        if ((i < m_element.size()) && (m_element[i]->getType() != Type::Invalid))
        {
            return m_element[i]->castTo<T>();
        }

        return setItem<T>(i);
    }
    template<typename T = Item>
    [[nodiscard]] [[nodiscard]] [[nodiscard]] [[nodiscard]] [[nodiscard]] [[nodiscard]] [[nodiscard]] auto
    getItem(unsigned i) const -> const T &
    {
        static const T defaultObject;

        if (i < m_element.size())
        {
            return m_element[i]->castTo<T>();
        }

        LOG_WARNING("Key #", i, " not found");
        return defaultObject;
    }
    auto getItems() -> std::vector<std::unique_ptr<Item>> & { return m_element; }
    [[nodiscard]] auto getItems() const -> const std::vector<std::unique_ptr<Item>> & { return m_element; }
    auto flow(std::ostream &os, unsigned level = 0) const -> std::ostream & override;
    auto flow(std::ostream &os,
              const std::string &newline,
              const std::string &indent,
              const std::string &termination,
              unsigned level) const -> std::ostream & override;
    explicit operator bool() const override { return true; }
};

class Node
{
private:
    Type m_type;
    std::string m_tag{};

public:
    Node(Type type, std::string tag): m_type(type), m_tag(std::move(tag)) {}
    [[nodiscard]] auto getType() const -> Type { return m_type; }
    [[nodiscard]] auto getName() const -> const std::string & { return m_tag; }
    [[nodiscard]] auto getId() const -> unsigned { return str2any<unsigned>(m_tag); }
};

using NodeList = std::vector<Node>;
using BrowsingCallback = std::function<void(const Item &, const NodeList &)>;

auto retrieve(const Object &root, const std::string &tagList) -> const Item &;
void browse(const Item &root, const BrowsingCallback &onAtomicItem);
auto merge(std::vector<Object> objectList) -> Object;

template<typename T>
auto Item::asVectorOf() const -> std::vector<T>
{
    if (m_type == Type::Array)
    {
        const auto &a = dynamic_cast<const Array &>(*this);
        std::vector<T> result;

        result.reserve(a.getSize());
        for (std::size_t i = 0U; i < a.getSize(); i++)
        {
            result.push_back(a.getItem(i).as<T>());
        }

        return result;
    }

    return {};
}

auto fromCommandLine(int argc, const char **argv) -> Object;
} // namespace JSON
} // namespace iloj::misc

auto operator<<(std::ostream &os, const iloj::misc::JSON::Object &o) -> std::ostream &;
