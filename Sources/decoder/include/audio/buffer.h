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

#include <iloj/media/descriptor.h>
#include <mutex>
#include <vector>

class AudioBuffer
{
private:
    std::mutex m_mutex;
    std::vector<float> m_interleavedSamples;
    bool m_muted = false;

public:
    AudioBuffer() = default;
    ~AudioBuffer() = default;
    AudioBuffer(const AudioBuffer &) = delete;
    AudioBuffer(AudioBuffer &&) noexcept = delete;
    auto operator=(const AudioBuffer &) -> AudioBuffer & = delete;
    auto operator=(AudioBuffer &&) noexcept -> AudioBuffer & = delete;
    void clear();
    void mute(bool b) { m_muted = b; }
    void push(unsigned formatId,
              unsigned packingId,
              unsigned nbChannels,
              unsigned sampleRate,
              const void *buffer,
              unsigned length);
    void pop(float *buffer, unsigned length);
};