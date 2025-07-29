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

#include <TMIV/MivBitstream/AccessUnit.h>
#include <iloj/gpu/types.h>
#include <iloj/media/descriptor.h>
#include <iloj/misc/packet.h>

using HANDLE = void *;

using DataDescriptor = iloj::media::Descriptor::Data;
using DataPacket = iloj::misc::Packet<DataDescriptor>;
using DataPacketList = std::vector<DataPacket>;

using AudioDescriptor = iloj::media::Descriptor::Audio;
using AudioPacket = iloj::misc::Packet<AudioDescriptor>;
using AudioInput = iloj::misc::Input<AudioDescriptor>;
using AudioOutput = iloj::misc::Output<AudioDescriptor>;

using VideoDescriptor = iloj::media::Descriptor::Video;
using VideoPacket = iloj::misc::Packet<VideoDescriptor>;
using VideoInput = iloj::misc::Input<VideoDescriptor>;
using VideoOutput = iloj::misc::Output<VideoDescriptor>;



enum VideoStream
{
    Occupancy = 0,
    Geometry,
    Texture,
    Transparency,
    Size
};

using VideoStreamType = decltype(VideoStream::Occupancy);

struct VpccPatchMetadata
{  
    uint16_t U0{0};
    uint16_t V0{0};
    uint16_t U1{0};
    uint16_t V1{0};
    uint16_t D1{0};
    uint16_t NormalAxis{0};
    uint16_t PatchOrientation{0};
    uint16_t ProjectionMode{0};
};

struct VpccMetadata
{
    int frame_index{-1};
    int frame_width;
    int frame_height;
    
    std::vector<VpccPatchMetadata> patchBlockBuffers = {};
    std::vector<size_t> blockToPatch = {};
};

using MivMetadata = TMIV::MivBitstream::AccessUnit;

struct GenericMetadata
{

    enum ContentType
    {
        Unknown = 0,
        MIV,
        VPCC
    };

    GenericMetadata() { 
        MIVMetadata = std::make_unique<MivMetadata>();
        VPCCMetadata = std::make_unique<VpccMetadata>();
    }

    GenericMetadata(VpccMetadata vpcc)
    {
        MIVMetadata = std::make_unique<MivMetadata>();
        VPCCMetadata = std::make_unique<VpccMetadata>(vpcc);
        contentType = VPCC;
    }

    GenericMetadata(MivMetadata miv)
    {
        MIVMetadata = std::make_unique<MivMetadata>(miv);
        VPCCMetadata = std::make_unique<VpccMetadata>();
        contentType = MIV;
    }

    

    std::unique_ptr<MivMetadata> MIVMetadata;
    std::unique_ptr<VpccMetadata> VPCCMetadata;

    int contentId{-1};
    int segmentId{-1};
    
    ContentType contentType{ContentType::Unknown};
};

using GenericMetadataPacket = iloj::misc::Packet<GenericMetadata>;

struct DecodedVideoData
{
    GenericMetadataPacket metadataPacket;
    std::array<VideoPacket, VideoStream::Size> videoPacketList;
};

using DecodedVideoPacket = iloj::misc::Packet<DecodedVideoData>;
using DecodedVideoInput = iloj::misc::Input<DecodedVideoData>;
using DecodedVideoOutput = iloj::misc::Output<DecodedVideoData>;
