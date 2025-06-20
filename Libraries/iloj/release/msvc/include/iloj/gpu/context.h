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

#include "memory.h"
#include "rtt.h"
#include <map>
#include <vector>
#include <thread>

#if defined(ILOJ_USE_DISCRETE_GPU) && defined(_WIN32)
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; // NOLINT
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;   // NOLINT
}
#endif

namespace iloj::gpu
{
class Context
{
    friend class Object::Base;

private:
    Object::Manager m_objectManager;
    unsigned m_defaultFrameBufferId = 0;
    RenderToTextureManager m_rttManager;
    Texture::SlotManager m_slotManager;
    static std::map<std::thread::id, std::vector<Context *>> g_bindingMap; // NOLINT
    static std::vector<Context *> g_allContexts;

public:
    Context();
    ~Context();
    Context(const Context &) = delete;
    Context(Context &&) = default;
    auto operator=(const Context &) -> Context & = delete;
    auto operator=(Context &&) -> Context & = default;
    
    static auto hasInstance() -> bool;
    static auto getInstance() -> Context &;
    static void enumerateAllContexts(std::function<void(Context* context, bool& stop)> contextHandler);
    void unregister();

    void setDefaultFrameBufferId(unsigned default_fbo_id) { m_defaultFrameBufferId = default_fbo_id; }
    [[nodiscard]] auto getDefaultFrameBufferId() const -> unsigned { return m_defaultFrameBufferId; }
    auto getSlotManager() -> Texture::SlotManager & { return m_slotManager; }
    template<typename T>
    void execute(T &target, const std::function<void()> &plan)
    {
        m_rttManager.getRenderToTexture<T>().execute(target, plan);
    }
    template<typename T>
    void execute(TargetList<T> targets, const std::function<void()> &plan)
    {
        m_rttManager.getRenderToTexture<T>().execute(std::move(targets), plan);
    }
    template<typename T>
    void execute(T &target, typename RenderToTexture<T>::DepthBufferType &depth, const std::function<void()> &plan)
    {
        m_rttManager.getRenderToTexture<T>().execute(target, depth, plan);
    }
    template<typename T>
    void execute(TargetList<T> targets,
                 typename RenderToTexture<T>::DepthBufferType &depth,
                 const std::function<void()> &plan)
    {
        m_rttManager.getRenderToTexture<T>().execute(targets, depth, plan);
    }
    void resolve(Texture2DMS &source, Texture2D &target);
    void readPixels(const Texture2D &source, void *ptr, unsigned format, unsigned type, bool clamp);
    void enable();
    static void disable();

private:
    auto getObjectManager() -> Object::Manager & { return m_objectManager; }
};
} // namespace iloj::gpu
