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

#include <limits>
#include <string>
#include <vector>

namespace iloj::misc
{
namespace FileSystem
{
class Path
{
private:
    std::string m_formattedPath;

public:
    Path() = default;
    ~Path() = default;
    Path(const std::string &path);
    Path(const Path &) = default;
    Path(Path &&) = default;
    auto operator=(const Path &) -> Path & = default;
    auto operator=(Path &&) -> Path & = default;
    auto operator=(const std::string &path) -> Path &;
    [[nodiscard]] auto toString() const -> const std::string & { return m_formattedPath; };
    [[nodiscard]] auto getParent() const -> Path;
    [[nodiscard]] auto getRelative(unsigned level = 1) const -> Path;
    static auto getWorkingDirectory() -> Path;
    static auto getTemporaryDirectory() -> const Path &;
    static auto getTemporary() -> Path;
    [[nodiscard]] auto isRelative() const -> bool;
    [[nodiscard]] auto isAbsolute() const -> bool;
    [[nodiscard]] auto isValid() const -> bool { return !(m_formattedPath.empty()); }
    static auto getProcess() -> Path;
    static auto getRelative(const Path &relativePath, const Path &referencePath = getWorkingDirectory()) -> Path;
    static auto getAbsolute(const Path &relativePath, const Path &referencePath = getWorkingDirectory()) -> Path;
    static auto getAbsolute(const std::vector<Path> &relativePathList, std::size_t level = 0) -> Path;

private:
    static auto format(const std::string &path) -> std::string;
};

class Item
{
public:
    enum class Type
    {
        None,
        File,
        Directory
    };

private:
    Path m_path;

public:
    Item() = default;
    explicit Item(Path path): m_path(std::move(path)) {}
    virtual ~Item() = default;
    Item(const Item &) = default;
    Item(Item &&) = default;
    auto operator=(const Item &) -> Item & = default;
    auto operator=(Item &&) -> Item & = default;
    [[nodiscard]] auto getType() const -> Type;
    [[nodiscard]] auto exist() const -> bool { return (getType() != Type::None); }
    [[nodiscard]] auto isFile() const -> bool { return (getType() == Type::File); }
    [[nodiscard]] auto isDirectory() const -> bool { return (getType() == Type::Directory); }
    [[nodiscard]] auto getPath() const -> const Path & { return m_path; }
    void setPath(Path path) { m_path = std::move(path); }
    auto move(Path dst) -> bool;
    static auto getStringFromType(Type type) -> std::string;
};

class File: public Item
{
public:
    using Item::Item;
    using Item::operator=;
    [[nodiscard]] auto getSize() const -> std::size_t;
    [[nodiscard]] auto getDirectoryPath() const -> Path { return getPath().getParent(); }
    [[nodiscard]] auto getName() const -> std::string;
    [[nodiscard]] auto getBaseName() const -> std::string;
    [[nodiscard]] auto getExtension() const -> std::string;
    [[nodiscard]] auto changeExtension(const std::string &extension) const -> File;
    [[nodiscard]] auto toString() const -> std::string;
    [[nodiscard]] auto toBuffer() const -> std::vector<std::uint8_t>;
    [[nodiscard]] auto fromString(const std::string &s) const -> bool;
    [[nodiscard]] auto splitName() const -> std::pair<std::string, std::string>;
    auto create() -> bool;
    auto remove() -> bool;
    [[nodiscard]] auto copy(const Path &dst) const -> bool;
};

class Directory: public Item
{
public:
    struct ScanMode
    {
        enum
        {
            None = 0,
            All = std::numeric_limits<int>::max()
        };
    };

private:
    std::vector<Directory> m_directories;
    std::vector<File> m_files;

public:
    Directory() = default;
    explicit Directory(Path path, int level = ScanMode::None): Item(std::move(path)) { scan(level); }
    ~Directory() override = default;
    Directory(const Directory &) = default;
    Directory(Directory &&) = default;
    auto operator=(const Directory &) -> Directory & = default;
    auto operator=(Directory &&) -> Directory & = default;
    [[nodiscard]] auto getSize() const -> std::size_t;
    [[nodiscard]] auto getDirectories() const -> const std::vector<Directory> & { return m_directories; }
    [[nodiscard]] auto getFiles() const -> const std::vector<File> & { return m_files; }
    auto addDirectory(const std::string &relativeName) -> Directory &
    {
        m_directories.emplace_back(Path::getAbsolute(Path{relativeName}, getPath()), 0);
        return m_directories.back();
    }
    auto addFile(const std::string &relativeName) -> File &
    {
        m_files.emplace_back(Path::getAbsolute(Path{relativeName}, getPath()));
        return m_files.back();
    }
    void scan(int level = std::numeric_limits<int>::max());
    auto create() -> bool;
    auto clear() -> bool;
    auto remove() -> bool;
    [[nodiscard]] auto copy(const Path &dst) const -> bool;
    void flow(std::ostream &os, unsigned maxDisplayedFile = 10, const std::string &pattern = "") const;

protected:
    auto create_empty() -> bool;
    auto remove_empty() -> bool;
};
} // namespace FileSystem
} // namespace iloj::misc

auto operator<<(std::ostream &os, const iloj::misc::FileSystem::Directory &dir) -> std::ostream &;
