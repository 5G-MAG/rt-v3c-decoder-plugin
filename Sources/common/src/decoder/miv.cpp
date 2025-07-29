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

#include <TMIV/Common/Bytestream.h>
#include <TMIV/Common/verify.h>
#include <TMIV/Decoder/DecodeAtlas.h>
#include <TMIV/Decoder/DecodeAtlasSubBitstream.h>
#include <TMIV/Decoder/DecodeCommonAtlas.h>
#include <TMIV/Decoder/DecodeNalUnitStream.h>
#include <TMIV/Decoder/DecodeV3cSampleStream.h>
#include <TMIV/Decoder/DecodeVideoSubBitstream.h>
#include <TMIV/Decoder/V3cUnitBuffer.h>
#include <common/decoder/miv.h>
#include <iloj/misc/logger.h>

using namespace iloj::misc;
using namespace iloj::media;

namespace miv
{
namespace tmiv
{
using namespace TMIV::Common;
using namespace TMIV::MivBitstream;
using namespace TMIV::PtlChecker;
} // namespace tmiv

auto getVideoStreamName(int videoStreamId) -> const std::string &
{
    static std::array<std::string, VideoStream::Size> videoStreamNameList = {
        "Occupancy", "Geometry", "Texture", "Transparency"};

    return videoStreamNameList[videoStreamId];
}

namespace
{
void setLoggingStrategy()
{
    tmiv::replaceLoggingStrategy(
        [](tmiv::LogLevel level, std::string_view message)
        {
            using tmiv::LogLevel;
            switch (level)
            {
                case LogLevel::error:
                    LOG_ERROR(message);
                    break;
                case LogLevel::warning:
                    LOG_WARNING(message);
                    break;
                case LogLevel::info:
                    [[fallthrough]]; // TMIV is quite verbose, so log info as debug level
                case LogLevel::verbose:
                    [[fallthrough]];
                case LogLevel::debug:
                    LOG_DEBUG(message);
                    break;
                case LogLevel::silent:
                    break;
                default:
                    break;
            }
        });
}

class NoPtlChecker: public tmiv::AbstractChecker
{
    // Inherited via AbstractChecker
    void replaceLogger(Logger value) override {}
    void checkVuh(const tmiv::V3cUnitHeader &vuh) override {}
    void checkNuh(const tmiv::NalUnitHeader &nuh) override {}
    void checkAndActivateVps(const tmiv::V3cParameterSet &vps) override {}
    void checkAndActivateCasps(const tmiv::CommonAtlasSequenceParameterSetRBSP &casps) override {}
    void checkAsps(tmiv::AtlasId atlasId, const tmiv::AtlasSequenceParameterSetRBSP &asps) override {}
    void checkAfps(const tmiv::AtlasFrameParameterSetRBSP &afps) override {}
    void checkAtl(const tmiv::NalUnitHeader &nuh, const tmiv::AtlasTileLayerRBSP &atl) override {}
    void checkCaf(const tmiv::NalUnitHeader &nuh, const tmiv::CommonAtlasFrameRBSP &caf) override {}
    void checkVideoFrame(tmiv::VuhUnitType vut,
                         const tmiv::AtlasSequenceParameterSetRBSP &asps,
                         const tmiv::Frame<> &frame) override
    {
    }
    void checkV3cFrame(const tmiv::AccessUnit &frame) override {}
};

void checkCapabilities(const TMIV::MivBitstream::AccessUnit &au)
{
    VERIFY_MIVBITSTREAM(
        au.vps.profile_tier_level().ptl_profile_toolset_idc() == tmiv::PtlProfileToolsetIdc::MIV_Main ||
        au.vps.profile_tier_level().ptl_profile_toolset_idc() == tmiv::PtlProfileToolsetIdc::MIV_Extended ||
        au.vps.profile_tier_level().ptl_profile_toolset_idc() == tmiv::PtlProfileToolsetIdc::MIV_Geometry_Absent);
    VERIFY_MIVBITSTREAM(au.vps.profile_tier_level().ptl_profile_reconstruction_idc() ==
                        tmiv::PtlProfileReconstructionIdc::Rec_Unconstrained);

    VERIFY_MIVBITSTREAM(au.vps.vpsMivExtensionPresentFlag());
    VERIFY_V3CBITSTREAM(au.vps.vps_extension_count() == 1);

    for (size_t k = 0; k <= au.vps.vps_atlas_count_minus1(); ++k)
    {
        const auto j = au.vps.vps_atlas_id(k);
        VERIFY_MIVBITSTREAM(au.vps.vps_map_count_minus1(j) == 0);
        VERIFY_MIVBITSTREAM(!au.vps.vps_auxiliary_video_present_flag(j));
    }
}

void decodeMvpl(TMIV::MivBitstream::AccessUnit &au, const tmiv::MivViewParamsList &mvpl, bool dqParamsPresentFlag)
{
    au.viewParamsList.assign(mvpl.mvp_num_views_minus1() + size_t{1}, {});

    for (uint16_t viewIdx = 0; viewIdx <= mvpl.mvp_num_views_minus1(); ++viewIdx)
    {
        auto &vp = au.viewParamsList[viewIdx];
        vp.viewId = mvpl.mvp_view_id(viewIdx);
        vp.pose = tmiv::Pose::decodeFrom(mvpl.camera_extrinsics(viewIdx));
        vp.viewInpaintFlag = mvpl.mvp_inpaint_flag(viewIdx);
        vp.ci = mvpl.camera_intrinsics(viewIdx);
        if (dqParamsPresentFlag)
        {
            vp.dq = mvpl.depth_quantization(viewIdx);
        }
        if (mvpl.mvp_pruning_graph_params_present_flag())
        {
            vp.pp = mvpl.pruning_parent(viewIdx);
        }

        vp.name = "view";
    }

    au.viewParamsList.constructViewIdIndex();
}

void decodeMvpue(TMIV::MivBitstream::AccessUnit &au, const tmiv::MivViewParamsUpdateExtrinsics &mvpue)
{
    for (uint16_t i = 0; i <= mvpue.mvpue_num_view_updates_minus1(); ++i)
    {
        au.viewParamsList[mvpue.mvpue_view_idx(i)].pose = tmiv::Pose::decodeFrom(mvpue.camera_extrinsics(i));
    }
}

void decodeMvpui(TMIV::MivBitstream::AccessUnit &au, const tmiv::MivViewParamsUpdateIntrinsics &mvpui)
{
    for (uint16_t i = 0; i <= mvpui.mvpui_num_view_updates_minus1(); ++i)
    {
        au.viewParamsList[mvpui.mvpui_view_idx(i)].ci = mvpui.camera_intrinsics(i);
    }
}

void decodeMvpudq(TMIV::MivBitstream::AccessUnit &au, const tmiv::MivViewParamsUpdateDepthQuantization &mvpudq)
{
    for (uint16_t i = 0; i <= mvpudq.mvpudq_num_view_updates_minus1(); ++i)
    {
        au.viewParamsList[mvpudq.mvpudq_view_idx(i)].dq = mvpudq.depth_quantization(i);
    }
}

void decodeViewParamsList(TMIV::MivBitstream::AccessUnit &au, const TMIV::Decoder::CommonAtlasAccessUnit &commonAtlasAu)
{
    const auto &caf = commonAtlasAu.caf;
    if (caf.caf_extension_present_flag() && caf.caf_miv_extension_present_flag())
    {
        const auto &came = caf.caf_miv_extension();
        bool dqParamsPresentFlag = true;
        if (commonAtlasAu.casps.casps_extension_present_flag() &&
            commonAtlasAu.casps.casps_miv_extension_present_flag())
        {
            dqParamsPresentFlag =
                commonAtlasAu.casps.casps_miv_extension().casme_depth_quantization_params_present_flag();
        }
        if (commonAtlasAu.foc == 0)
        {
            decodeMvpl(au, came.miv_view_params_list(), dqParamsPresentFlag);
        }
        else
        {
            if (came.came_update_extrinsics_flag())
            {
                decodeMvpue(au, came.miv_view_params_update_extrinsics());
            }
            if (came.came_update_intrinsics_flag())
            {
                decodeMvpui(au, came.miv_view_params_update_intrinsics());
            }
            if (commonAtlasAu.casps.casps_miv_extension().casme_depth_quantization_params_present_flag() &&
                came.came_update_depth_quantization_flag() && dqParamsPresentFlag)
            {
                decodeMvpudq(au, came.miv_view_params_update_depth_quantization());
            }
        }
    }

    if (commonAtlasAu.casps.casps_extension_present_flag() && commonAtlasAu.casps.casps_miv_extension_present_flag())
    {
        const auto &casme = commonAtlasAu.casps.casps_miv_extension();
        if (casme.casme_vui_params_present_flag())
        {
            const auto &vui = casme.vui_parameters();
            VERIFY_MIVBITSTREAM(!au.vui || *au.vui == vui);
            au.vui = vui;
        }
    }
}

void decodeCommonAtlas(TMIV::MivBitstream::AccessUnit &au, const TMIV::Decoder::CommonAtlasAccessUnit &commonAtlasAu)
{
    decodeViewParamsList(au, commonAtlasAu);
    au.gup = commonAtlasAu.gup;
    au.vs = commonAtlasAu.vs;
    au.vcp = commonAtlasAu.vcp;
    au.vp = commonAtlasAu.vp;
    au.casps = commonAtlasAu.casps;
}

auto decodeBlockToPatchMap(TMIV::MivBitstream::AccessUnit &au, size_t k, const tmiv::PatchParamsList &ppl)
    -> tmiv::Frame<tmiv::PatchIdx>
{
    const auto &asps = au.atlas[k].asps;

    const std::int32_t log2PatchPackingBlockSize = asps.asps_log2_patch_packing_block_size();
    const auto patchPackingBlockSize = 1 << log2PatchPackingBlockSize;
    const auto offset = patchPackingBlockSize - 1;

    const auto atlasBlockToPatchMapWidth = (asps.asps_frame_width() + offset) / patchPackingBlockSize;
    const auto atlasBlockToPatchMapHeight = (asps.asps_frame_height() + offset) / patchPackingBlockSize;

    // All elements of TileBlockToPatchMap are first initialized to -1 as follows [9.2.6]
    auto btpm = tmiv::Frame<tmiv::PatchIdx>::lumaOnly({atlasBlockToPatchMapWidth, atlasBlockToPatchMapHeight});
    btpm.fillValue(tmiv::unusedPatchIdx);

    // Then the AtlasBlockToPatchMap array is updated as follows:
    for (size_t p = 0; p < ppl.size(); ++p)
    {
        const size_t xOrg = ppl[p].atlasPatch2dPosX() / patchPackingBlockSize;
        const size_t yOrg = ppl[p].atlasPatch2dPosY() / patchPackingBlockSize;
        const size_t atlasPatchWidthBlk = (ppl[p].atlasPatch2dSizeX() + offset) / patchPackingBlockSize;
        const size_t atlasPatchHeightBlk = (ppl[p].atlasPatch2dSizeY() + offset) / patchPackingBlockSize;

        for (size_t y = 0; y < atlasPatchHeightBlk; ++y)
        {
            for (size_t x = 0; x < atlasPatchWidthBlk; ++x)
            {
                if (!asps.asps_patch_precedence_order_flag() ||
                    btpm.getPlane(0)(yOrg + y, xOrg + x) == tmiv::unusedPatchIdx)
                {
                    btpm.getPlane(0)(yOrg + y, xOrg + x) = static_cast<uint16_t>(p);
                }
            }
        }
    }

    return btpm;
}

auto decodePatchParamsList(const TMIV::Decoder::AtlasAccessUnit &atlasAu,
                           const tmiv::V3cParameterSet &vps,
                           tmiv::AtlasId atlasId,
                           tmiv::PatchParamsList &ppl) -> const tmiv::PatchParamsList &
{
    VERIFY_MIVBITSTREAM(atlasAu.atlV.size() == 1);
    const auto &ath = atlasAu.atlV.front().atlas_tile_header();

    VERIFY_MIVBITSTREAM(ath.ath_type() == tmiv::AthType::I_TILE || ath.ath_type() == tmiv::AthType::SKIP_TILE);
    if (ath.ath_type() == tmiv::AthType::SKIP_TILE)
    {
        return ppl;
    }

    const auto &atdu = atlasAu.atlV.front().atlas_tile_data_unit();
    const auto &asps = atlasAu.asps;
    const auto &afps = atlasAu.afps;

    const auto tilePartitions = std::vector{{tmiv::TilePartition{.partitionPosX = 0,
                                                                 .partitionPosY = 0,
                                                                 .partitionWidth = asps.asps_frame_width(),
                                                                 .partitionHeight = asps.asps_frame_height()}}};

    ppl.assign(atdu.atduTotalNumberOfPatches(), {});

    for (size_t p = 0; p < ppl.size(); ++p)
    {
        const auto &pin = atdu.patch_information_data(p);
        const auto &pdu = pin.patch_data_unit();
        ppl[p] = tmiv::PatchParams::decodePdu(pdu, vps, atlasId, asps, afps, ath, {});
    }

    return ppl;
}

void decodeAtlas(TMIV::MivBitstream::AccessUnit &au, const TMIV::Decoder::AtlasAccessUnit &atlasAu, size_t k)
{
    au.atlas[k].asps = atlasAu.asps;
    au.atlas[k].afps = atlasAu.afps;
    const auto atlasId = au.vps.vps_atlas_id(k);
    const auto &ppl = decodePatchParamsList(atlasAu, au.vps, atlasId, au.atlas[k].patchParamsList);
    requireAllPatchesWithinProjectionPlaneBounds(au.viewParamsList, ppl);
    au.atlas[k].blockToPatchMap = decodeBlockToPatchMap(au, k, ppl);
}

auto decodeVideoPayload(const tmiv::V3cUnitHeader &vuh, std::shared_ptr<TMIV::Decoder::V3cUnitBuffer> inputBuffer)
    -> std::string
{
    // Adapt the V3C unit buffer to a video sub-bitstream source
    const auto videoSubBitstreamSource = [&inputBuffer, vuh]() -> std::string
    {
        if (auto v3cUnit = (*inputBuffer)(vuh))
        {
            return v3cUnit->v3c_unit_payload().video_sub_bitstream().data();
        }

        return {};
    };

    // Decode video payload
    std::ostringstream hevc_payload;

    for (auto buffer = videoSubBitstreamSource(); !buffer.empty();)
    {
        // Decode the NAL unit size
        // NOTE(#494): For V3C, LengthSizeMinusOne is equal to 3.
        std::istringstream stream{buffer.substr(0, 4)};
        const auto size = tmiv::getUint32(stream);

        // Decode the payload bytes
        auto payload = buffer.substr(4, size);
        VERIFY_V3CBITSTREAM(payload.size() == size);

        // HEVC payload
        tmiv::putUint8(hevc_payload, 0);
        tmiv::putUint8(hevc_payload, 0);
        tmiv::putUint8(hevc_payload, 0);
        tmiv::putUint8(hevc_payload, 1);
        hevc_payload << payload;

        // Update buffer
        buffer = buffer.substr(4 + size);

        if (buffer.empty())
        {
            buffer = videoSubBitstreamSource();
        }
    }

    return hevc_payload.str();
}

} // namespace

auto decodeMivBuffer(std::string inputData)
    -> std::pair<TMIV::MivBitstream::AccessUnit, std::array<DataPacket, VideoStream::Size>>
{
    setLoggingStrategy();

    const auto checker = std::make_shared<NoPtlChecker>();

    std::istringstream inputStream{std::move(inputData)};
    auto vssDecoder = TMIV::Decoder::decodeV3cSampleStream(inputStream);
    static constexpr auto onVps = [](auto &&...) {};
    auto inputBuffer = std::make_shared<TMIV::Decoder::V3cUnitBuffer>(std::move(vssDecoder), onVps);

    TMIV::MivBitstream::AccessUnit au;
    //= make_packet<GenericMetadata>();
    DataPacket occupancyDataPacket;
    DataPacket geometryDataPacket;
    DataPacket textureDataPacket;
    DataPacket transparencyDataPacket;

    //auto &au = mivPacket.getContent();

    if (auto vuVPS = (*inputBuffer)(tmiv::V3cUnitHeader::vps()))
    {
        // IRAP
        au.foc = 0;

        // Decode VPS
        au.vps = vuVPS->v3c_unit_payload().v3c_parameter_set();
        checkCapabilities(au);

        // Decode common atlas data
        auto vpsId = au.vps.vps_v3c_parameter_set_id();
        auto vuhCAD = tmiv::V3cUnitHeader::cad(vpsId);
        auto commonAtlasDecoder = TMIV::Decoder::decodeCommonAtlas(
            TMIV::Decoder::decodeAtlasSubBitstream(TMIV::Decoder::atlasSubBitstreamSource(inputBuffer, vuhCAD)),
            checker);

        if (auto commonAtlasAu = commonAtlasDecoder())
        {
            decodeCommonAtlas(au, *commonAtlasAu);
        }
        else
        {
            LOG_ERROR("Common atlas data decoding failed");
            return {};
        }

        // Decode atlas data
        // NOTE: Decode only first atlas
        au.vps.vps_atlas_count_minus1(0);
        for (size_t k = 0; k <= au.vps.vps_atlas_count_minus1(); ++k)
        {
            const auto atlasId = au.vps.vps_atlas_id(k);
            auto vuhAD = tmiv::V3cUnitHeader::ad(vpsId, atlasId);

            auto atlasDecoder = TMIV::Decoder::decodeAtlas(
                TMIV::Decoder::decodeAtlasSubBitstream(TMIV::Decoder::atlasSubBitstreamSource(inputBuffer, vuhAD)),
                vuhAD,
                checker);

            au.atlas.emplace_back();

            if (auto atlasAu = atlasDecoder())
            {
                decodeAtlas(au, *atlasAu, k);
            }
            else
            {
                LOG_ERROR("Atlas data #", k, " decoding failed");
                return {};
            }
        }

        // Demux occupancy / texture / geometry / transparency video streams
        for (size_t k = 0; k <= au.vps.vps_atlas_count_minus1(); ++k)
        {
            const auto atlasId = au.vps.vps_atlas_id(k);

            // Occupancy video stream
            if (au.vps.vps_occupancy_video_present_flag(atlasId))
            {
                auto vuhOVD = tmiv::V3cUnitHeader::ovd(vpsId, atlasId);

                auto data = decodeVideoPayload(vuhOVD, inputBuffer);

                if (!data.empty())
                {
                    auto ptr = reinterpret_cast<const std::uint8_t *>(data.data());
                    occupancyDataPacket =
                        make_packet<Descriptor::Data>(Descriptor::Data::container_type{ptr, ptr + data.size()});
                }
                else
                {
                    LOG_ERROR("Invalid occupancy data");
                }
            }

            // Geometry video stream
            if (au.vps.vps_geometry_video_present_flag(atlasId))
            {
                auto vuhGVD = tmiv::V3cUnitHeader::gvd(vpsId, atlasId);

                auto data = decodeVideoPayload(vuhGVD, inputBuffer);

                if (!data.empty())
                {
                    auto ptr = reinterpret_cast<const std::uint8_t *>(data.data());
                    geometryDataPacket =
                        make_packet<Descriptor::Data>(Descriptor::Data::container_type{ptr, ptr + data.size()});
                }
                else
                {
                    LOG_ERROR("Invalid geometry data");
                }
            }

            // Attribute video streams
            const auto &ai = au.vps.attribute_information(atlasId);

            for (std::uint8_t attributeIndex = 0; attributeIndex < ai.ai_attribute_count(); ++attributeIndex)
            {
                auto vuhAVD = tmiv::V3cUnitHeader::avd(vpsId, atlasId, attributeIndex);

                std::string data = decodeVideoPayload(vuhAVD, inputBuffer);

                if (!data.empty())
                {
                    auto ptr = reinterpret_cast<const std::uint8_t *>(data.data());
                    auto type = ai.ai_attribute_type_id(attributeIndex);

                    if (type == tmiv::AiAttributeTypeId::ATTR_TEXTURE)
                    {
                        textureDataPacket =
                            make_packet<Descriptor::Data>(Descriptor::Data::container_type{ptr, ptr + data.size()});
                    }
                    else if (type == tmiv::AiAttributeTypeId::ATTR_TRANSPARENCY)
                    {
                        transparencyDataPacket =
                            make_packet<Descriptor::Data>(Descriptor::Data::container_type{ptr, ptr + data.size()});
                    }
                }
                else
                {
                    LOG_ERROR("Invalid attribute data");
                    return {};
                }
            }
        }
    }

    return {std::move(au),
            {std::move(occupancyDataPacket),
             std::move(geometryDataPacket),
             std::move(textureDataPacket),
             std::move(transparencyDataPacket)}};
}
} // namespace miv
