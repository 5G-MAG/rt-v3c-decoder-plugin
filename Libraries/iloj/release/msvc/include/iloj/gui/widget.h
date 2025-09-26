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

#include <iloj/gpu/interactor.h>
#include <iloj/gpu/viewport.h>
#include <iloj/gui/types.h>

namespace iloj::gui
{
class Widget
{
    template<typename W>
    friend class Delegate;
    friend class Interactor;

private:
    Widget *m_root = nullptr;

protected:
    Widget *m_parent = nullptr;
    std::vector<std::unique_ptr<Widget>> m_children;
    bool m_hasFocus = false;
    bool m_isHovered = false;
    int m_left = 0, m_bottom = 0;
    int m_width = 0, m_height = 0;
    bool m_visible = true;
    bool m_active = true;
    bool m_movable = true;
    bool m_resizable = true;
    int m_alignment = Alignment::None;
    int m_marginLeft = 0, m_marginRight = 0;
    int m_marginBottom = 0, m_marginTop = 0;

    std::function<void()> m_onDrawCallback;
    std::function<void(int, int, int, int)> m_onResizeCallback;
    std::function<void(void)> m_afterResizeCallback;
    std::function<bool(gpu::Interactor::MouseButton, int, int)> m_onMouseButtonPressCallback;
    std::function<void(gpu::Interactor::MouseButton, int, int)> m_onMouseButtonReleaseCallback;
    std::function<void(int, int)> m_onMouseMoveCallback;
    std::function<void(int, int)> m_onMouseEnteringCallback;
    std::function<void(int, int)> m_onMouseLeavingCallback;
    std::function<bool(unsigned short)> m_onKeyPressCallback;
    std::function<bool(unsigned short)> m_onKeyReleaseCallback;
    std::function<bool(int, int)> m_onScrollCallback;
    std::function<bool(int, void *)> m_onCustomEventCallback;
    std::function<bool(const std::vector<std::string> &)> m_onDragOnDropCallback;

public:
    Widget() = default;
    virtual ~Widget() = default;
    Widget(const Widget &) = delete;
    Widget(Widget &&) = default;
    auto operator=(const Widget &) -> Widget & = delete;
    auto operator=(Widget &&) -> Widget & = default;
    auto root() -> Widget &;
    [[nodiscard]] auto hasParent() const -> bool { return (m_parent != nullptr); }
    [[nodiscard]] auto parent() const -> const Widget & { return *m_parent; }
    void addChild(std::unique_ptr<Widget> w)
    {
        m_children.push_back(std::move(w));
        m_children.back()->setParent(this);
    }
    template<typename W, typename... Args>
    auto addChild(Args &&... args) -> W &
    {
        addChild(std::make_unique<W>(std::forward<Args>(args)...));
        return dynamic_cast<W &>(*m_children.back());
    }
    auto children() -> std::vector<std::unique_ptr<Widget>> & { return m_children; }
    [[nodiscard]] auto children() const -> const std::vector<std::unique_ptr<Widget>> & { return m_children; }
    template<typename W>
    auto child(std::size_t id) -> W &
    {
        return dynamic_cast<W &>(*m_children[id]);
    }
    template<typename W>
    auto child(std::size_t id) const -> const W &
    {
        return dynamic_cast<const W &>(*m_children[id]);
    }
    void setFocus(bool b);
    [[nodiscard]] auto hasFocus() const -> bool { return m_hasFocus; }
    void discardFocus();
    [[nodiscard]] auto isHovered() const -> bool { return m_isHovered; }
    void setMovable(bool b) { m_movable = b; }
    [[nodiscard]] auto isMovable() const -> bool { return m_movable; }
    void move(int left, int bottom)
    {
        if (m_movable)
        {
            m_left = left;
            m_bottom = bottom;
        }
    }
    [[nodiscard]] auto left() const -> int { return m_left; }
    [[nodiscard]] auto bottom() const -> int { return m_bottom; }
    [[nodiscard]] auto x() const -> int { return (m_parent != nullptr) ? (m_parent->x() + m_left) : m_left; }
    [[nodiscard]] auto y() const -> int { return (m_parent != nullptr) ? (m_parent->y() + m_bottom) : m_bottom; }
    void align();
    void setAlignment(int v) { m_alignment = v; }
    [[nodiscard]] auto getAlignment() const -> bool { return (m_alignment != 0); }
    void setMargin(int left, int right, int bottom, int top)
    {
        m_marginLeft = left;
        m_marginRight = right;
        m_marginBottom = bottom;
        m_marginTop = top;
    }
    void setResizable(bool b) { m_resizable = b; }
    [[nodiscard]] auto isResizable() const -> bool { return m_resizable; }
    void resize(int width, int height)
    {
        if (m_resizable)
        {
            m_width = width;
            m_height = height;
        }
    }
    void setFixedSize(int width, int height)
    {
        if (m_resizable)
        {
            m_width = width;
            m_height = height;

            m_resizable = false;
        }
    }
    [[nodiscard]] auto width() const -> int { return m_width; }
    [[nodiscard]] auto height() const -> int { return m_height; }
    [[nodiscard]] auto getViewPort() const -> iloj::gpu::ViewPort
    {
        return {static_cast<unsigned>(width()),
                static_cast<unsigned>(height()),
                static_cast<unsigned>(x()),
                static_cast<unsigned>(y())};
    }
    void setVisible(bool b) { m_visible = b; }
    [[nodiscard]] auto isVisible() const -> bool { return m_visible; }
    void setActive(bool b, bool recursive = false)
    {
        m_active = b;

        if (recursive)
        {
            for (auto &widget : m_children)
            {
                widget->setActive(b, true);
            }
        }
    }
    [[nodiscard]] auto isActive() const -> bool { return m_active; }
    void setOnDrawCallback(const std::function<void()> &f) { m_onDrawCallback = f; }
    void setOnResizeCallback(const std::function<void(int, int, int, int)> &f) { m_onResizeCallback = f; }
    void setAfterResizeCallback(const std::function<void(void)> &f) { m_afterResizeCallback = f; }
    void setOnMouseButtonPressCallback(const std::function<bool(gpu::Interactor::MouseButton, int, int)> &f)
    {
        m_onMouseButtonPressCallback = f;
    }
    void setOnMouseButtonReleaseCallback(const std::function<void(gpu::Interactor::MouseButton, int, int)> &f)
    {
        m_onMouseButtonReleaseCallback = f;
    }
    void setOnMouseMoveCallback(const std::function<void(int, int)> &f) { m_onMouseMoveCallback = f; }
    void setOnMouseLeavingCallback(const std::function<void(int, int)> &f) { m_onMouseLeavingCallback = f; }
    void setOnKeyPressCallback(const std::function<bool(unsigned short)> &f) { m_onKeyPressCallback = f; }
    void setOnKeyReleaseCallback(const std::function<bool(unsigned short)> &f) { m_onKeyReleaseCallback = f; }
    void setOnScrollCallback(const std::function<bool(int, int)> &f) { m_onScrollCallback = f; }
    void setOnCustomEventCallback(const std::function<bool(int, void *)> &f) { m_onCustomEventCallback = f; }
    void setOnDragAndDropFunction(const std::function<bool(const std::vector<std::string> &)> &f)
    {
        m_onDragOnDropCallback = f;
    }
    [[nodiscard]] auto isInside(int x, int y, bool relative = true) const -> bool
    {
        if (relative)
        {
            return ((0 <= x) && (x < m_width) && (0 <= y) && (y < m_height));
        }

        return ((m_left <= x) && (x < (m_left + m_width)) && (m_bottom <= y) && (y < (m_bottom + m_height)));
    }

private:
    void setParent(Widget *parent) { m_parent = parent; }
    void updateAlignment();
    void drawTree();
    void onResizeTree(int w_old, int h_old, int w_new, int h_new);
    auto onMouseButtonPressTree(gpu::Interactor::MouseButton button, int x, int y) -> bool;
    void onMouseButtonReleaseTree(gpu::Interactor::MouseButton button, int x, int y);
    auto onMouseMoveTree(int x, int y) -> bool;
    void onMouseEnteringTree(int x, int y);
    auto onKeyPressTree(unsigned short key) -> bool;
    auto onKeyReleaseTree(unsigned short key) -> bool;
    auto onScrollTree(int dx, int dy) -> bool;
    auto onCustomEventTree(unsigned id, void *data) -> bool;
    auto onDragAndDropTree(const std::vector<std::string> &draggedItems) -> bool;

protected:
    virtual void draw()
    {
        if (m_onDrawCallback)
        {
            m_onDrawCallback();
        }
    }
    virtual void onResize(int w_old, int h_old, int w_new, int h_new)
    {
        if (m_onResizeCallback)
        {
            m_onResizeCallback(w_old, h_old, w_new, h_new);
        }
    }
    virtual void afterResize()
    {
        if (m_afterResizeCallback)
        {
            m_afterResizeCallback();
        }
    }
    virtual auto onMouseButtonPress(gpu::Interactor::MouseButton button, int x, int y) -> bool
    {
        return m_onMouseButtonPressCallback ? m_onMouseButtonPressCallback(button, x, y) : false;
    }
    virtual void onMouseButtonRelease(gpu::Interactor::MouseButton button, int x, int y)
    {
        if (m_onMouseButtonReleaseCallback)
        {
            m_onMouseButtonReleaseCallback(button, x, y);
        }
    }
    virtual void onMouseMove(int x, int y)
    {
        if (m_onMouseMoveCallback)
        {
            m_onMouseMoveCallback(x, y);
        }
    }
    virtual void onMouseEntering(int x, int y)
    {
        if (m_onMouseEnteringCallback)
        {
            m_onMouseEnteringCallback(x, y);
        }
    }
    virtual void onMouseLeaving(int x, int y)
    {
        if (m_onMouseLeavingCallback)
        {
            m_onMouseLeavingCallback(x, y);
        }
    }
    virtual auto onKeyPress(unsigned short key) -> bool
    {
        return m_onKeyPressCallback ? m_onKeyPressCallback(key) : false;
    }
    virtual auto onKeyRelease(unsigned short key) -> bool
    {
        return m_onKeyReleaseCallback ? m_onKeyReleaseCallback(key) : false;
    }
    virtual auto onScroll(int dx, int dy) -> bool { return m_onScrollCallback ? m_onScrollCallback(dx, dy) : false; }
    virtual auto onCustomEvent(unsigned id, void *data) -> bool
    {
        return m_onCustomEventCallback ? m_onCustomEventCallback(id, data) : false;
    }
    virtual auto onDragAndDrop(const std::vector<std::string> &draggedItems) -> bool
    {
        return m_onDragOnDropCallback ? m_onDragOnDropCallback(draggedItems) : false;
    }
};

} // namespace iloj::gui
