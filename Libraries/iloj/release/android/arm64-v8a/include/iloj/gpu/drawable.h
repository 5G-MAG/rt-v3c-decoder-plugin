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

#include "context.h"
#include "memory.h"
#include "program.h"

namespace iloj::gpu
{
class Drawable: public Object::Base
{
private:
    unsigned m_vao{std::numeric_limits<unsigned>::max()}, m_faceMode = 0;
    std::unique_ptr<Buffer::Index> m_indices;
    std::map<std::string, std::unique_ptr<Buffer::Vertex>> m_buffers;
    iloj::misc::Reference<Program> m_program{};

public:
    Drawable();
    Drawable(const Drawable &) = delete;
    Drawable(Drawable &&d) noexcept;
    ~Drawable() override;
    auto operator=(const Drawable &) -> Drawable & = delete;
    auto operator=(Drawable && /*d*/) noexcept -> Drawable &;
    template<typename... ENTRIES>
    Drawable(unsigned faceMode, unsigned nbFace, ENTRIES &&... next)
    {
        glGenVertexArrays(1, &m_vao);
        setFaceMode(faceMode);
        setIndices(std::make_unique<Buffer::Index>(getVerticesPerFace(faceMode) * nbFace));
        setBuffer(std::forward<ENTRIES>(next)...);
    }
    void setFaceMode(unsigned mode) { m_faceMode = mode; }
    void setIndices(std::unique_ptr<Buffer::Index> buffer) { m_indices = std::move(buffer); }
    auto getIndices() -> Buffer::Index & { return *m_indices; }
    void addBuffer(const std::string &name, std::unique_ptr<Buffer::Vertex> a) { m_buffers[name] = std::move(a); }
    auto getBuffer(const std::string &name) -> Buffer::Vertex & { return *m_buffers[name]; }
    void draw(Program &program, int instanceCount = 0);
    
    void drawIndirect(Program &program, const void* indirect);
    static auto getVerticesPerFace(unsigned faceMode) -> unsigned;

protected:
    void destroy() override { cleanup(); }
    void setBuffer() {}
    template<typename T>
    void setAtomicBuffer(const Buffer::Vertex::Entry<T> &entry)
    {
        addBuffer(entry.getName(),
                  std::make_unique<Buffer::Vertex>(
                      std::vector<T>(m_indices->getNumberOfIndices(), entry.getDefaultValue()), entry.getMode()));
    }
    template<typename T, typename... ENTRIES>
    void setBuffer(const Buffer::Vertex::Entry<T> &entry, ENTRIES... next)
    {
        setAtomicBuffer(entry);
        setBuffer(next...);
    }

private:
    void cleanup();
};
} // namespace iloj::gpu
