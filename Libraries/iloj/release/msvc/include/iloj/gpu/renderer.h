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

#include "benchmarker.h"
#include "delegate.h"
#include "interactor.h"
#include "processor.h"
#include "scheduler.h"
#include "viewport.h"

namespace iloj::gpu
{
class Renderer: public Processor
{
private:
    mutable iloj::misc::SpinLock m_pAccess;
    Scheduler m_scheduler;
    Benchmarker m_benchmarker;
    std::vector<std::unique_ptr<Interactor>> m_interactor;
    std::vector<std::unique_ptr<Delegate>> m_delegate;
    bool m_isVisible = false, m_sizeUpdate = true, m_isInitialized = false;
    ViewPort m_viewPort;

public:
    Renderer(): m_scheduler(*this) {}
    auto getViewPort() const -> const ViewPort & { return m_viewPort; }
    void startRendering();
    void stopRendering();
    auto getScheduler() -> Scheduler & { return m_scheduler; }
    auto getBenchmarker() -> Benchmarker & { return m_benchmarker; }
    auto hasDelegate() const -> bool { return !m_delegate.empty(); }
    auto getNumberOfDelegate() const -> unsigned { return m_delegate.size(); }
    auto getDelegate(unsigned i = 0) -> Delegate & { return *m_delegate[i]; }
    template<typename D>
    auto getDelegate(unsigned i = 0) -> D &
    {
        return dynamic_cast<D &>(*m_delegate[i]);
    }
    template<typename D, typename... ARGS>
    auto addDelegate(ARGS &&... args) -> D &
    {
        execute([&]() { addDelegate(std::make_unique<D>(std::forward<ARGS>(args)...)); });
        return dynamic_cast<D &>(*m_delegate.back());
    }
    auto hasInteractor() const -> bool { return !m_interactor.empty(); }
    auto getNumberOfInteractor() const -> unsigned { return m_interactor.size(); }
    auto getInteractor(unsigned i = 0) -> Interactor & { return *m_interactor[i]; }
    template<typename I>
    auto getInteractor(unsigned i = 0) -> I &
    {
        return dynamic_cast<I &>(*m_interactor[i]);
    }
    void addInteractor(std::unique_ptr<Interactor> interactor)
    {
        interactor->attach(*this);
        m_interactor.push_back(std::move(interactor));
    }
    template<typename I, typename... ARGS>
    auto addInteractor(ARGS &&... args) -> I &
    {
        addInteractor(std::make_unique<I>(std::forward<ARGS>(args)...));
        return dynamic_cast<I &>(*m_interactor.back());
    }
    void render();

protected:
    virtual void swapGLBuffers() {}
    virtual void populate()
    {
        for (auto &d : m_delegate)
        {
            d->populate();
        }
    }
    virtual void draw()
    {
        for (auto &d : m_delegate)
        {
            d->update();
        }
    }
    virtual void reshape(int w, int h)
    {
        for (auto &d : m_delegate)
        {
            d->reshape(w, h);
        }
    }

protected:
    void onDeleteEvent();
    void onShowEvent();
    void onHideEvent();
    void onResizeEvent(int w, int h);

private:
    void addDelegate(std::unique_ptr<Delegate> delegate)
    {
        delegate->attach(*this);
        m_delegate.push_back(std::move(delegate));
        m_delegate.back()->bind();
    }
};
} // namespace iloj::gpu
