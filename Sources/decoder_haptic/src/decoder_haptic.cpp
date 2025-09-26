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

#include <decoder_haptic.h>

#include <common/misc/types_haptic.h>
#include <iloj/misc/packet.h>

#include <IOHaptics/include/IOJson.h>
#include <Synthesizer/include/Helper.h>
#include <Tools/include/InputParser.h>
#include <Tools/include/OHMData.h>
#include <Types/include/Haptics.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <chrono>

using haptics::io::IOJson;
using haptics::synthesizer::Helper;
using haptics::tools::InputParser;
using haptics::types::Haptics;
using namespace iloj::misc;

////////////////////////////////////////////////////////////////////////////////////////////////////
DecoderHaptic::DecoderHaptic(std::chrono::duration<double> initTime)
{ 
    m_InitTime = initTime;
}

void DecoderHaptic::decode(std::string s, HapticInput& hapticInput)
{
    Haptics hapticFile;
    // Load the .hjif
    bool loadingSuccess = true;
    rapidjson::Document jsonTree;
    if (jsonTree.Parse(s.c_str()).HasParseError())
    {
        std::cerr << "Invalid HJIF input file: JSON parsing error" << std::endl;
        return;
    }
    if (!jsonTree.IsObject())
    {
        std::cerr << "Invalid HJIF input file: not a JSON object" << std::endl;
        return;
    }
    if (!(jsonTree.HasMember("version") && jsonTree.HasMember("profile") && jsonTree.HasMember("level") &&
          jsonTree.HasMember("date") && jsonTree.HasMember("description") && jsonTree.HasMember("timescale") &&
          jsonTree.HasMember("perceptions") && jsonTree["perceptions"].IsArray() && jsonTree.HasMember("avatars") &&
          jsonTree["avatars"].IsArray() && jsonTree.HasMember("syncs") && jsonTree["syncs"].IsArray()))
    {
        std::cerr << "Invalid HJIF input file: missing required field" << std::endl;
        return;
    }

    auto version = std::string(jsonTree["version"].GetString());
    auto profile = std::string(jsonTree["profile"].GetString());
    auto level = static_cast<uint8_t>(jsonTree["level"].GetUint());
    auto date = std::string(jsonTree["date"].GetString());
    auto description = std::string(jsonTree["description"].GetString());
    hapticFile.setVersion(version);
    hapticFile.setProfile(profile);
    hapticFile.setLevel(level);
    hapticFile.setDate(date);
    hapticFile.setDescription(description);
    if (jsonTree.HasMember("timescale") && jsonTree["timescale"].IsInt())
    {
        hapticFile.setTimescale(jsonTree["timescale"].GetInt());
    }
    loadingSuccess = loadingSuccess && IOJson::loadAvatars(jsonTree["avatars"], hapticFile);
    loadingSuccess = loadingSuccess && IOJson::loadPerceptions(jsonTree["perceptions"], hapticFile);
    loadingSuccess = loadingSuccess && IOJson::loadSyncs(jsonTree["syncs"], hapticFile);

    if (!loadingSuccess) 
        return;

    hapticFile.linearize();

    // Transform haptic file to events
    if (hapticFile.getPerceptionsSize() > 0)
    {
        // Get only 1 perception for smartphone devices
        auto perception = hapticFile.getPerceptionAt(0);
        for (auto channelId = 0ULL; (channelId < perception.getChannelsSize()) && (channelId < 2); channelId++)
        {
            // Get only 2 1st channels for smartphone or VR devices
            auto channel = perception.getChannelAt(channelId);
            for (auto bandId = 0ULL; bandId < channel.getBandsSize(); bandId++)
            {
                // Get only Vectorial band for devices
                auto band = channel.getBandAt(bandId);
                if (band.getBandType() == haptics::types::BandType::Transient)
                {
                    // Get effect (normally only 1)
                    for (auto effectId = 0ULL; effectId < band.getEffectsSize(); effectId++)
                    {
                        auto effect = band.getEffectAt(effectId);
                        // Divide the effect keyframes to multiple vibration events if needed
                        auto keyframeId = 0;
                        while (keyframeId < effect.getKeyframesSize())
                        {
                            auto keyframe = effect.getKeyframeAt(keyframeId);
                            if (keyframe.getAmplitudeModulation().has_value() &&
                                keyframe.getAmplitudeModulation().value() > 0.2)
                            {
                                HapticPacket packet = make_packet<HapticDescriptor>();
                                HapticDescriptor &desc = packet.getContent();
                                desc.setChannelId(channel.getId());
                                desc.setStartTimeStamp(
                                    m_InitTime + std::chrono::duration<double>{effect.getPosition() / 1000.f} +
                                    std::chrono::duration<double>{keyframe.getRelativePosition().value() / 1000.f});
                                desc.setEndTimeStamp(m_InitTime +
                                                     std::chrono::duration<double>{effect.getPosition() / 1000.f} +
                                                     std::chrono::duration<double>{
                                                         0.022 + keyframe.getRelativePosition().value() / 1000.f});
                                desc.setStartIntensity(keyframe.getAmplitudeModulation().value());
                                desc.setEndIntensity(keyframe.getAmplitudeModulation().value());
                                // m_hapticDecoder->getHapticInput().push(packet);
                                hapticInput.insert(
                                    packet,
                                    [](const HapticPacket &a, const HapticPacket &b) {
                                        return (a.getContent().getStartTimeStamp() <=
                                                b.getContent().getStartTimeStamp());
                                    });
                            }
                            keyframeId++;
                        }
                    }
                }
                else if (band.getBandType() == haptics::types::BandType::VectorialWave)
                {
                    // Get effects
                    for (auto effectId = 0ULL; effectId < band.getEffectsSize(); effectId++)
                    {
                        auto effect = band.getEffectAt(effectId);
                        // Divide the effect keyframes to multiple vibration events if needed
                        auto keyframeId = 0;
                        int lastKeyframeTime = 0;
                        float lastKeyframeAmplitude = 1;
                        while (keyframeId < effect.getKeyframesSize())
                        {
                            auto keyframe = effect.getKeyframeAt(keyframeId);
                            if (keyframe.getAmplitudeModulation().has_value() &&
                                keyframe.getAmplitudeModulation().value() > 0.2)
                            {
                                if ((keyframe.getRelativePosition().value() - lastKeyframeTime) > 10)
                                {
                                    HapticPacket packet = make_packet<HapticDescriptor>();
                                    HapticDescriptor &desc = packet.getContent();
                                    desc.setChannelId(channel.getId());
                                    desc.setStartTimeStamp(
                                        m_InitTime +
                                        std::chrono::duration<double>{effect.getPosition() / 1000.f} +
                                        std::chrono::duration<double>{lastKeyframeTime / 1000.f});
                                    desc.setEndTimeStamp(
                                        m_InitTime +
                                        std::chrono::duration<double>{effect.getPosition() / 1000.f} +
                                        std::chrono::duration<double>{keyframe.getRelativePosition().value() / 1000.f});
                                    desc.setStartIntensity(lastKeyframeAmplitude);
                                    if (keyframe.getAmplitudeModulation().has_value())
                                        desc.setEndIntensity(keyframe.getAmplitudeModulation().value());
                                    else
                                        desc.setEndIntensity(lastKeyframeAmplitude);
                                    // m_hapticDecoder->getHapticInput().push(packet);
                                    hapticInput.insert(
                                        packet,
                                        [](const HapticPacket &a, const HapticPacket &b) {
                                            return (a.getContent().getStartTimeStamp() <=
                                                    b.getContent().getStartTimeStamp());
                                        });
                                }
                            }
                            // Even if the keyframe is skipped
                            lastKeyframeTime = keyframe.getRelativePosition().value();
                            if (keyframe.getAmplitudeModulation().has_value())
                                lastKeyframeAmplitude = keyframe.getAmplitudeModulation().value();
                            keyframeId++;
                        }
                    }
                }
            }
        }
    }
}
