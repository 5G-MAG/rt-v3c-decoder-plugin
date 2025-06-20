#pragma once

#include <iloj/media/colorspace.h>
#include <iloj/media/descriptor.h>
#include <iloj/media/avcodec.h>
#include <iloj/gpu/processor.h>
#include <iloj/misc/json.h>
#include <iloj/misc/memory.h>
#include <iloj/misc/packet.h>
#include <map>
#include <set>

namespace iloj::media::AVCodec
{
    class IIlojAVCodec: public iloj::misc::Service
    {
    public:
        IIlojAVCodec() = default;
        IIlojAVCodec(const IIlojAVCodec &) = delete;
        IIlojAVCodec(IIlojAVCodec &&) = delete;
        virtual ~IIlojAVCodec() = default;
        auto operator=(const IIlojAVCodec &) -> IIlojAVCodec & = delete;
        auto operator=(IIlojAVCodec &&) -> IIlojAVCodec & = delete;

        virtual auto setJavaVirtualMachine(void *vm) -> bool = 0;

        virtual void init(bool bLogs) = 0;
        virtual auto open(std::string path,
                  const std::vector<int> &stream_idx = {-2, -1},
                  std::vector<unsigned> m_factorysize = {},
                  unsigned streaming_size = 4096)  -> bool = 0;
        virtual auto open(std::string path,
                  unsigned width,
                  unsigned height,
                  const std::string &pixelFormat,
                  int frameRate,
                  unsigned factorySize = 25) -> bool = 0;

        virtual auto is_open() const -> bool = 0;
        virtual auto getBestAudioStreamId() const -> int = 0;
        virtual auto getBestVideoStreamId() const -> int = 0;
        virtual void setOnOpeningFunction(const std::function<void()> &f) = 0;
        virtual void setOnCompletionFunction(const std::function<void()> &f) = 0;
        virtual void setOnChunkCompletionFunction(const std::function<void()> &f) = 0;
        virtual auto getInformation() -> iloj::misc::JSON::Object = 0;
        virtual auto getStreamingInput() -> iloj::misc::Input<Descriptor::Data> & = 0;
        virtual auto getAudioOutput(unsigned id) -> iloj::misc::Output<Descriptor::Audio> & = 0;
        virtual void setOnAudioFrameCallback(unsigned id, const std::function<void(Descriptor::Audio &)> &callback) = 0;

        virtual auto getVideoOutput(unsigned id,
                                    int nbThread,
                                    bool hardwareDecoding,
                                    std::string androidFormat,
                                    iloj::gpu::Processor &ctx)
            -> iloj::misc::Output<Descriptor::Video> & = 0;
    };
} // namespace iloj::media::AVCodec
