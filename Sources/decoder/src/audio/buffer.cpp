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

#include <audio/buffer.h>
#include <iloj/media/descriptor.h>
#include <iloj/misc/logger.h>

void AudioBuffer::clear()
{
    std::lock_guard<std::mutex> guard{m_mutex};
    m_interleavedSamples.clear();
}

void AudioBuffer::push(unsigned formatId,
                       unsigned packingId,
                       unsigned nbChannels,
                       unsigned sampleRate,
                       const void *buffer,
                       unsigned length)
{
    using namespace iloj::media;

    std::lock_guard<std::mutex> guard{m_mutex};

    if ((1 <= nbChannels) && (sampleRate == 48000) &&
        (static_cast<Descriptor::Audio::PackingId>(packingId) == Descriptor::Audio::PackingId::Planar) &&
        (static_cast<Descriptor::Audio::FormatId>(formatId) == Descriptor::Audio::FormatId::FLT))
    {
        auto nbSamplesPerChannel = length / (nbChannels * sizeof(float));
        auto nbSamples = 2 * nbSamplesPerChannel;

        m_interleavedSamples.resize(m_interleavedSamples.size() + nbSamples);

        auto iter = m_interleavedSamples.end() - nbSamples;

        auto ptr1 = reinterpret_cast<const float *>(buffer);
        auto ptr2 = reinterpret_cast<const float *>(buffer) + (nbChannels - 1) * nbSamplesPerChannel;

        for (auto id = 0ULL; id < nbSamplesPerChannel; id++)
        {
            *iter++ = ptr1[id];
            *iter++ = ptr2[id];
        }
    }
    else
    {
        LOG_ERROR("Not supported audio stream");
        // TODO
    }
}

void AudioBuffer::pop(float *buffer, unsigned length)
{
    std::lock_guard<std::mutex> guard{m_mutex};
    auto nbSamples = 2 * length;

    if (nbSamples <= m_interleavedSamples.size())
    {
        //LOG_INFO("Pop ", nbSamples, " with ", m_interleavedSamples.size(), "total samples in buffer");

        if (m_muted)
        {
            std::fill(buffer, buffer + nbSamples, 0.F);
        }
        else
        {
            std::copy(m_interleavedSamples.begin(), m_interleavedSamples.begin() + nbSamples, buffer);
        }

        std::copy(m_interleavedSamples.begin() + nbSamples, m_interleavedSamples.end(), m_interleavedSamples.begin());
        m_interleavedSamples.resize(m_interleavedSamples.size() - nbSamples);
    }
    else
    {
        
        if (m_interleavedSamples.size() > 0)
        {
            /*LOG_INFO(
                "Pop ", nbSamples, " with ", m_interleavedSamples.size(), "total samples in buffer, padding expected");*/
            std::copy(m_interleavedSamples.begin(), m_interleavedSamples.begin() + m_interleavedSamples.size(), buffer); //Copy all the remaining samples in the buffer
            std::fill(buffer + m_interleavedSamples.size(), buffer + nbSamples, 0.F); //Pad with 0s
            /*std::copy(
                m_interleavedSamples.begin() + nbSamples, m_interleavedSamples.end(), m_interleavedSamples.begin());*/
            m_interleavedSamples.resize(0);
        }
        else
        {
            /*LOG_INFO(
                "Pop ", nbSamples, " without any sample, outputing 0s");*/
            std::fill(buffer, buffer + nbSamples, 0.F);
        }
        
    }
}