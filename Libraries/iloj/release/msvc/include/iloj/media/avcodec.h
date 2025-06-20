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

#include <iloj/media/colorspace.h>
#include <iloj/media/descriptor.h>
#include <iloj/gpu/processor.h>
#include <iloj/misc/json.h>
#include <iloj/misc/memory.h>
#include <iloj/misc/packet.h>
#include <map>
#include <set>

namespace iloj::media::AVCodec
{
    class IIlojAVCodec;
    class CIlojAVcodecs;

    class Decoder
    {
    public:
        struct Stream
        {
        public:
            enum
            {
                All = -3,
                BestAudio = -2,
                BestVideo = -1
            };
            enum class Type
            {
                Unknown = 0,
                Audio,
                Video,
                Data
            };
        };

    public:
        Decoder() = default;
        Decoder(const Decoder &) = delete;
        Decoder(Decoder &&) = delete;
        virtual ~Decoder() = default;
        auto operator=(const Decoder &) -> Decoder & = delete;
        auto operator=(Decoder &&) -> Decoder & = delete;

        void init(std::string avcodec_name);
        void exit();
        auto open(std::string path,
                  const std::vector<int> &stream_idx = {Stream::BestAudio, Stream::BestVideo},
                  std::vector<unsigned> m_factorysize = {},
                  unsigned streaming_size = 4096) -> bool;
        auto open(std::string path,
                  unsigned width,
                  unsigned height,
                  const std::string &pixelFormat,
                  int frameRate,
                  unsigned factorySize = 25) -> bool;

        auto is_open() const -> bool;
        auto getBestAudioStreamId() const -> int;
        auto getBestVideoStreamId() const -> int;
        void setOnOpeningFunction(const std::function<void()> &f);
        void setOnCompletionFunction(const std::function<void()> &f);
        void setOnChunkCompletionFunction(const std::function<void()> &f);

        auto getInformation() -> iloj::misc::JSON::Object;
        auto getStreamingInput() -> iloj::misc::Input<Descriptor::Data> &;
        auto getAudioOutput(unsigned id) -> iloj::misc::Output<Descriptor::Audio> &;
        void setOnAudioFrameCallback(unsigned id, const std::function<void(Descriptor::Audio &)> &callback);
        auto getVideoOutput(unsigned id,
                            int nbThread,
                            bool hardwareDecoding,
                            std::string androidFormat,
                            iloj::gpu::Processor &ctx)
            -> iloj::misc::Output<Descriptor::Video> &;

        void start();
        void stop();
        void finish();

    private:
        CIlojAVcodecs* m_codecs;
        IIlojAVCodec* m_codec;
    };
    } // namespace iloj::media::AVCodec
