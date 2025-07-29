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

#include <map>
#include <string>

using namespace std;

namespace iloj::media::AVCodec
{
class IIlojAVCodec;

typedef IIlojAVCodec *(*funcCreateAVCodec)();

class CIlojAVcodecs
{

public:
    CIlojAVcodecs();
    virtual ~CIlojAVcodecs();

    void init(string avcodec_name);
    void exit();

    const map<string, IIlojAVCodec *> &getAVCodecs() { return m_mapAVCodec; }

private:
    map<string, IIlojAVCodec *> m_mapAVCodec;
};
} // namespace iloj::media::AVCodec
