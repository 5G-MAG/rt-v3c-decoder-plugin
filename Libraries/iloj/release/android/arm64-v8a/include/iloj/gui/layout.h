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

#include "widget.h"
#include <iloj/gpu/types.h>

namespace iloj::gui
{
class Layout: public Widget
{
public:
    enum class Packing
    {
        Span,
        Fit
    };

protected:
    gpu::Mat<unsigned> m_occupancyMap;
    std::vector<std::pair<gpu::Vec2u, gpu::Vec2u>> m_layout;
    Packing m_packing = Packing::Span;

public:
    Layout(std::size_t m, std::size_t n);
    void setPacking(Packing p) { m_packing = p; }
    [[nodiscard]] auto getPacking() const -> Packing { return m_packing; }
    void addChild(unsigned i, unsigned j, std::unique_ptr<Widget> child, unsigned m = 1, unsigned n = 1);
    template<typename W, unsigned I, unsigned J, unsigned M, unsigned N, typename... Args>
    auto addChild(Args &&... args) -> W &
    {
        addChild(I, J, std::make_unique<W>(std::forward<Args>(args)...), M, N);
        return dynamic_cast<W &>(*children().back());
    }
    template<typename W, unsigned I, unsigned J>
    auto child() -> W &
    {
        return dynamic_cast<W &>(*m_children[m_occupancyMap(I, J)]);
    }

protected:
    void onResize(int w_old, int h_old, int w_new, int h_new) override;
    void afterResize() override;

private:
    [[nodiscard]] auto getOccupancyMap() const -> gpu::Mat<int>;
};

} // namespace iloj::gui
