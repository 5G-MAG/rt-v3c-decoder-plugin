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

#include "attribute.h"
#include "memory.h"
#include "uniform.h"
#include <map>
#include <memory>

namespace iloj::gpu
{
void glLogError(const std::string &s);

/////////////////////////////////////////////////////////////////////////
class Shader: public Object::Base
{
private:
    unsigned m_type{};
    unsigned m_id{std::numeric_limits<unsigned>::max()};

public:
    Shader(unsigned type, const std::string &code);
    ~Shader() override;
    Shader(const Shader &) = delete;
    Shader(Shader && /*other*/) noexcept;
    auto operator=(const Shader &) -> Shader & = delete;
    auto operator=(Shader && /*other*/) noexcept -> Shader &;
    [[nodiscard]] auto getId() const -> unsigned { return m_id; }
    [[nodiscard]] auto getType() const -> unsigned { return m_type; }
    static auto fromFile(unsigned type, const std::string &path) -> std::unique_ptr<Shader>;
    void update(const std::string &code);

protected:
    void destroy() override { cleanup(); }

private:
    void cleanup();
    void check(const std::string &code) const;
};

/////////////////////////////////////////////////////////////////////////
class Program: public Object::Base
{
public:
    enum class ShaderType
    {
        Vertex,
        TessControl,
        TessEvaluation,
        Geometry,
        Fragment,
        Compute
    };

private:
    int m_id = -1;
    std::unique_ptr<Shader> m_computeShader;
    std::unique_ptr<Shader> m_vertexShader, m_tessControlShader, m_tessEvaluationShader, m_geometryShader,
        m_fragmentShader;
    std::map<std::string, std::unique_ptr<Attribute>> m_attributes;
    std::map<std::string, std::unique_ptr<Uniform::Regular>> m_regularUniforms;
    std::map<std::string, std::unique_ptr<Uniform::Buffer>> m_bufferUniforms;

public:
    Program() = default;
    Program(const std::string &computeShader);
    Program(const std::string &vertex_code, const std::string &fragment_code);
    Program(const std::string &vertex_code, const std::string &geometry_code, const std::string &fragment_code);
    Program(const std::string &vertex_code,
            const std::string &tess_control_code,
            const std::string &tess_eval_code,
            const std::string &geometry_code,
            const std::string &fragment_code);
    Program(const std::string &vertex_code,
            const std::string &tess_control_code,
            const std::string &tess_eval_code,
            const std::string &geometry_code,
            const std::string &fragment_code,
            const std::string &extension_code);
    Program(const std::vector<std::pair<ShaderType, std::string>> &shaderList, const std::string &extension_code);
    Program(const Program &) = delete;
    Program(Program && /*p*/) noexcept;
    ~Program() override;
    auto operator=(const Program &) -> Program & = delete;
    auto operator=(Program && /*p*/) noexcept -> Program &;
    void bind() const;
    static void unbind();
    [[nodiscard]] auto isValid() const -> bool { return (-1 < m_id); }
    auto getAttribute(const std::string &name) -> Attribute &
    {
        auto iter = m_attributes.find(name);
        return (iter != m_attributes.end()) ? *(iter->second) : Attribute::Default();
    }
    auto getUniform(const std::string &name) -> Uniform::Base &;
    void setUniform() {}
    template<typename T, std::enable_if_t<Uniform::Type<T>::isRegular, int> = 0>
    void setAtomicUniform(const gpu::Uniform::Entry<T> &entry)
    {
        getRegularUniform(entry.first).setValue(entry.second);
    }
    template<typename T, std::enable_if_t<Uniform::Type<T>::isBuffer, int> = 0>
    void setAtomicUniform(const Uniform::Entry<T> &entry)
    {
        getBufferUniform(entry.first).setValue(entry.second);
    }
    template<typename T, typename... ENTRIES>
    void setUniform(const gpu::Uniform::Entry<T> &entry, ENTRIES &&... next)
    {
        setAtomicUniform(entry);
        setUniform(std::forward<ENTRIES>(next)...);
    }
    auto getRegularUniformList() const -> const std::map<std::string, std::unique_ptr<Uniform::Regular>> &
    {
        return m_regularUniforms;
    }
    auto getRegularUniform(const std::string &name) -> Uniform::Regular &
    {
        auto iter = m_regularUniforms.find(name);
        return (iter != m_regularUniforms.end()) ? *(iter->second) : Uniform::Regular::Default();
    }
    auto getBufferUniformList() const -> const std::map<std::string, std::unique_ptr<Uniform::Buffer>> &
    {
        return m_bufferUniforms;
    }
    auto getBufferUniform(const std::string &name) -> Uniform::Buffer &
    {
        auto iter = m_bufferUniforms.find(name);
        return (iter != m_bufferUniforms.end()) ? *(iter->second) : Uniform::Buffer::Default();
    }
    static auto Default() -> Program &
    {
        static Program p;
        return p;
    }

protected:
    void destroy() override { cleanup(); }

private:
    void cleanup();

private:
    void parseAttribute(const std::string &vertex_code);
    void parseRegularUniform(const std::vector<std::string> &shader_list);
    void parseBufferUniform(const std::vector<std::string> &shader_list);
    void addAttribute(const std::string &name);
    void addRegularUniform(const std::string &name);
    void addBufferUniform(const std::string &name, unsigned mode = GL_DYNAMIC_DRAW);

private:
    Program(std::unique_ptr<Shader> computeShader);
    Program(std::unique_ptr<Shader> vertexShader,
            std::unique_ptr<Shader> tessControlShader,
            std::unique_ptr<Shader> tessEvaluationShader,
            std::unique_ptr<Shader> geometryShader,
            std::unique_ptr<Shader> fragmentShader);
    void link() const;
};
} // namespace iloj::gpu
