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

#include "PCCBitstream.h"
#include "PCCBitstreamReader.h"
#include "PCCContext.h"
#include "PCCDecoder.h"
#include "PCCFrameContext.h"
#include "PCCHighLevelSyntax.h"
#include "PCCPatch.h"
#include "PCCSampleStreamV3CUnit.h"

#include <common/decoder/vpcc.h>
#include <iloj/misc/logger.h>

using namespace iloj::misc;
using namespace iloj::media;
using namespace pcc;

void exportToFile(std::string /*path*/, uint8_t * /*data*/, size_t /*size*/)
{
    //TODO
}


auto getVpccVideoStreamName(int videoStreamId) -> const std::string &
{
    static std::array<std::string, VideoStream::Size> videoStreamNameList = {
        "Occupancy", "Geometry", "Texture", "Transparency"};

    return videoStreamNameList[videoStreamId];
}

auto decodeVpccBuffer(std::vector<uint8_t> &inputData)
    -> std::pair<std::vector<VpccMetadata>, std::array<DataPacket, VideoStream::Size>>
{
    std::vector<VpccMetadata> framesMetadata;
    DataPacket occupancyDataPacket;
    DataPacket geometryDataPacket;
    DataPacket textureDataPacket;
    DataPacket transparencyDataPacket;

    pcc::PCCDecoder decoder;
    pcc::PCCBitstream bitstream;
    pcc::PCCBitstreamStat bitstreamStat;
    pcc::PCCBitstreamReader bitstreamReader;
    pcc::SampleStreamV3CUnit ssvu;
    size_t headerSize;

    
    bitstream.initialize(inputData);
    bitstreamStat.setHeader(bitstream.size());
    headerSize = bitstreamReader.read(bitstream, ssvu);
    bitstreamStat.incrHeader(headerSize);

    pcc::PCCContext context;
    context.setBitstreamStat(bitstreamStat);
    bitstreamReader.decode(ssvu, context);
    //auto &metadata = vpccPacket.getContent();

    auto& pccVps = context.getVps();
    context.resizeAtlas((size_t) pccVps.getAtlasCountMinus1() + 1);

    for (uint32_t atlId = 0; atlId < pccVps.getAtlasCountMinus1() + 1; atlId++)
    {
        context.getAtlas(atlId).allocateVideoFrames(context, 0);
        // first allocating the structures, frames will be added as the V3C
        // units are being decoded ???
        context.setAtlasIndex(atlId);
        decoder.createPatchFrameDataStructure(context);
    }

    // TODO : we get only atlas #0
    //uint16_t frameWidth = pccVps.getFrameWidth(0);
    //uint16_t frameHeight =pccVps.getFrameHeight(0);

    // TODO : not yet considered dual layers
    //bool isSingleLayer = pccVps.getMapCountMinus1(0) == 0; // TODO cas single layer
    // TODO : Send warning if not single layer

    //size_t fsize = context.getFrames().size();

    // TODO : this parser has to be updated as memory requirements seem to be far too heavy
    /*size_t widthOCM = 0, heightOCM = 0;
    size_t widthGEO = 0, heightGEO = 0;
    size_t widthTXT = 0, heightTXT = 0;*/

    auto & ocm_stream = context.getVideoBitstream(pcc::VIDEO_OCCUPANCY);
    ocm_stream.sampleStreamToByteStream();
    uint8_t *ocmPtr = ocm_stream.buffer();
    size_t ocmSize = ocm_stream.size();
    occupancyDataPacket = make_packet<Descriptor::Data>(Descriptor::Data::container_type{ocmPtr, ocmPtr + ocmSize});

    auto &geo_stream = context.getVideoBitstream(pcc::VIDEO_GEOMETRY);
    geo_stream.sampleStreamToByteStream();
    uint8_t *geoPtr = geo_stream.buffer();
    size_t geoSize = geo_stream.size();
    geometryDataPacket = make_packet<Descriptor::Data>(Descriptor::Data::container_type{geoPtr, geoPtr + geoSize});

    auto &txt_stream = context.getVideoBitstream(pcc::VIDEO_ATTRIBUTE);
    txt_stream.sampleStreamToByteStream();
    uint8_t *txtPtr = txt_stream.buffer();
    size_t txtSize = txt_stream.size();
    textureDataPacket = make_packet<Descriptor::Data>(Descriptor::Data::container_type{txtPtr, txtPtr + txtSize});

    // size_t gobPointLocalReconstructionModes = context.getPointLocalReconstructionModeNumber();

    //const uint16_t blockAttributeSize = sizeof(int16_t);
    //const uint16_t blockAttributes = 9;

    int frame_index = 0;

    for (auto &frame : context.getFrames())
    {
        const size_t numPatches = frame.getTile(0).getPatches().size();

        std::vector<VpccPatchMetadata> framePatchMetadataList;
        //uint16_t activeBlocks = 0;
        std::vector<size_t> blockToPatch;
        if (numPatches > 0)
        {
            // TODO : assumes same occupancy resolution for all patches in frame
            // we should be able to manage per patch occupancy resolution ?
            const size_t &occupancyResolution = frame.getTile(0).getPatches()[0].getOccupancyResolution();
            const size_t blockToPatchWidth = frame.getTile(0).getWidth() / occupancyResolution;
            const size_t blockToPatchHeight = frame.getTile(0).getHeight() / occupancyResolution;
            const size_t blockCount = blockToPatchWidth * blockToPatchHeight;

            blockToPatch.resize(blockCount, 0);

            for (size_t patchIdx = 0; patchIdx < numPatches; ++patchIdx)
            {
                const auto &patch = frame.getTile(0).getPatches()[patchIdx];

                // not used lod patch.getLod();
                // not used gopPointLocalReconstructionEnabled
                // no patchBlockFiltering
                size_t size_v0 = patch.getSizeV0();
                size_t size_u0 = patch.getSizeU0();

                for (size_t v0 = 0; v0 < size_v0; ++v0)
                {
                    for (size_t u0 = 0; u0 < size_u0; ++u0)
                    {
                        const int32_t blockIndex =
                            patch.patchBlock2CanvasBlock(u0, v0, blockToPatchWidth, blockToPatchHeight);
                        blockToPatch[blockIndex] = patchIdx + 1;
                    }
                }
            }

            // MetadataToTexture(frameGroup, frame, blockToPatch, blockToPatchWidth, blockToPatchHeight);

            framePatchMetadataList.reserve(sizeof(VpccPatchMetadata) * numPatches);

            for (size_t patchIdx = 0; patchIdx < numPatches; ++patchIdx)
            {
                const auto &patch = frame.getTile(0).getPatches()[patchIdx];
                VpccPatchMetadata m;
                //m.frame_index= frame_index; // was 0, is probably reserved
                m.U0 = patch.getU0();
                m.V0 = patch.getV0();
                m.U1 = patch.getU1();
                m.V1 = patch.getV1();
                m.D1 = patch.getD1();
                m.NormalAxis = patch.getNormalAxis();
                m.PatchOrientation = patch.getPatchOrientation();
                m.ProjectionMode = patch.getProjectionMode();
                framePatchMetadataList.push_back(std::move(m));
            }

            VpccMetadata metadata;
            metadata.frame_index = frame_index;
            metadata.frame_height = frame.getTile(0).getHeight();
            metadata.frame_width = frame.getTile(0).getWidth();

            metadata.blockToPatch = std::move(blockToPatch);
            metadata.patchBlockBuffers = std::move(framePatchMetadataList);
            framesMetadata.push_back(std::move(metadata));
            frame_index++;
        }

      /*  metadata.blockToPatch.push_back(std::move(blockToPatch));
        metadata.patchBlockBuffers.push_back(std::move(framePatchMetadataList));*/
        //metadata.patchBlockSizes.push_back(activeBlocks);
    }

    LOG_DEBUG("V-PCC parsing Done.");

    return {std::move(framesMetadata),
            {std::move(occupancyDataPacket),
             std::move(geometryDataPacket),
             std::move(textureDataPacket),
             std::move(transparencyDataPacket)}}; //Null
}


