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

#include <audio/audio.h>
#include <client/meta.h>
#include <client/reader.h>
#include <decoder/decoder.h>
#include <fstream>
#include <iloj/misc/filesystem.h>
#include <iloj/misc/logger.h>
#include <scheduler/scheduler.h>
#include <video/video.h>
#include <haptic/haptic.h>

static std::ofstream g_logStream;

void onCreation(const std::string &configFile)
{
    using namespace iloj::misc;

    g_logStream.open(FileSystem::Path{configFile}.getParent().toString() + "/V3CImmersiveDecoderVideo.log");

    Logger::getInstance().setStream(g_logStream);
    Logger::getInstance().setLevel(Logger::Level::Info);
    Logger::getInstance().enableFlushOnAppend(true);

    LOG_INFO("onCreation");
}

void onDestroy()
{
    LOG_INFO("onDestroy");
    g_logStream.close();
}

auto allocateClientInterface() -> std::unique_ptr<Client::Interface>
{
    return std::make_unique<MetaInterface>();
}

auto allocateDecoderInterface() -> std::unique_ptr<DecoderInterface::Interface>
{
    return std::make_unique<DecoderInterface>();
}

auto allocateSchedulerInterface() -> std::unique_ptr<Scheduler::Interface>
{
    return std::make_unique<SchedulerInterface>();
}

auto allocateAudioInterface() -> std::unique_ptr<Audio::Interface> { return std::make_unique<AudioInterface>(); }

auto allocateVideoInterface() -> std::unique_ptr<Video::Interface> { return std::make_unique<VideoInterface>(); }

auto allocateHapticInterface() -> std::unique_ptr<Haptic::Interface> { return std::make_unique<HapticInterface>(); }