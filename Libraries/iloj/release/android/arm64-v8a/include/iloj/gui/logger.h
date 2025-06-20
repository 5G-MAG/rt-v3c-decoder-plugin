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

#include "label.h"

namespace iloj::gui
{
class Logger: public Label
{
protected:
    std::ostringstream m_oss;
    //    int m_maxNumberOfLines = 5

public:
    auto getStream() -> std::ostringstream & { return m_oss; }

protected:
    void draw() override
    {
        setText(m_oss.str());
        Label::draw();
    }
};

} // namespace iloj::gui