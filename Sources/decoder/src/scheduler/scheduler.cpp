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

#include <iloj/misc/json.h>
#include <iloj/misc/packet.h>
#include <scheduler/scheduler.h>

using namespace iloj::misc;

////////////////////////////////////////////////////////////////////////////////////////////////////
std::chrono::duration<double> SchedulerInterface::MasterClock::now()
{
    std::chrono::duration<double> out = std::chrono::system_clock::now().time_since_epoch();
    if (m_forceDecodersSynchro) out = out - m_offset;
    return out;
}

void SchedulerInterface::MasterClock::updateOffset(std::chrono::milliseconds offset) { m_offset += offset;}

////////////////////////////////////////////////////////////////////////////////////////////////////
void SchedulerInterface::AudioScheduler::initialize()
{
    LOG_INFO("SchedulerInterface::AudioScheduler::initialize");

    if (m_input.wait())
    {
        const auto &desc = m_input.front().getContent();

        if (m_audioInterface)
        {
            auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(
                (desc.getMetadata().getTimeStamp() - m_latency) - m_masterClock->now());
                //std::chrono::system_clock::now().time_since_epoch());

            LOG_INFO("First audio delay: ", dt.count());

            std::this_thread::sleep_for(dt);

            m_audioInterface->onSampleEvent(m_input.front());
        }

        m_input.pop();
    }
}

void SchedulerInterface::AudioScheduler::idle()
{
    try
    {
        if (m_input.wait())
        {
           
            const auto &desc = m_input.front().getContent();
            auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(desc.getMetadata().getTimeStamp()- m_masterClock->now());

            if (dt.count() < 0)
            {
                m_masterClock->updateOffset(-dt);
                LOG_WARNING("Delay Audio: ", std::abs(dt.count()), "ms");
            }
            if (dt < m_latency)
            {
                if (m_audioInterface)
                {
                    m_audioInterface->onSampleEvent(m_input.front());
                 
                }
                m_input.pop();


            }
            
        }
    }
    catch (std::exception e)
    {
        LOG_ERROR(e.what());
        throw;
    }
}

void SchedulerInterface::AudioScheduler::finalize() { LOG_INFO("SchedulerInterface::AudioScheduler::finalize"); }

////////////////////////////////////////////////////////////////////////////////////////////////////
void SchedulerInterface::VideoScheduler::initialize()
{
    LOG_INFO("SchedulerInterface::VideoScheduler::initialize");
    m_masterClock->reset();
}

void SchedulerInterface::VideoScheduler::idle()
{
    try
    {
        if (m_input.wait())
        {
            const auto &desc = m_input.front().getContent();
            /*auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(
                desc.videoPacketList[VideoStream::Texture]->getMetadata().getTimeStamp() -
                std::chrono::system_clock::now().time_since_epoch());*/

            auto pts = desc.videoPacketList[VideoStream::Texture]->getMetadata().getTimeStamp();
            //std::chrono::duration<double> now = std::chrono::system_clock::now().time_since_epoch();
            std::chrono::duration<double> now = m_masterClock->now();

            auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(pts - now);

            if (dt.count() < 0)
            {
                m_masterClock->updateOffset(-dt);
                LOG_WARNING("Delay Videos: ", std::abs(dt.count()), "ms, global offset is: ", m_masterClock->getOffset().count(), "ms");
            }
            if (dt < m_jitter)
            {
                /*LOG_INFO("VIDEO PTS:",
                         m_masterClock->getTimeRelative(pts).count(),
                         " NOW: ",
                         m_masterClock->getTimeRelative(now).count(),
                         " DT: ",
                         dt.count(),
                         "ms");*/
                if (m_videoInterface)
                {
                    m_videoInterface->onSampleEvent(m_input.front());
                }

                m_input.pop();
            }
        }
    }
    catch (std::exception e)
    {
        LOG_ERROR(e.what());
        throw;
    }
}

void SchedulerInterface::VideoScheduler::finalize() { LOG_INFO("SchedulerInterface::VideoScheduler::finalize"); }

////////////////////////////////////////////////////////////////////////////////////////////////////

void SchedulerInterface::HapticScheduler::initialize() { LOG_INFO("SchedulerInterface::HapticScheduler::initialize"); }

void SchedulerInterface::HapticScheduler::idle()
{
    try
    {
         if (m_input.wait())
         {
            const auto &desc = m_input.front().getContent();

            auto pts = desc.getStartTimeStamp(); /*-m_latency;*/
            std::chrono::duration<double> now = m_masterClock->now(); 
            // std::chrono::system_clock::now().time_since_epoch();

            auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(pts - now);

            if (dt.count() < 0)
            {
                m_masterClock->updateOffset(-dt);
                LOG_WARNING("Delay Haptics: ", std::abs(dt.count()), "ms");
            }

            if (dt < m_latency)
            {
                /*LOG_INFO("HAPTICS PTS:",
                         m_masterClock->getTimeRelative(pts).count(),
                         " NOW: ",
                         m_masterClock->getTimeRelative(now).count(),
                         " DT: ",
                         dt.count(),
                         "ms");*/

                if (m_hapticInterface)
                {
                    //std::this_thread::sleep_for(dt);

                    m_hapticInterface->onSampleEvent(m_input.front());
                }

                m_input.pop();
            }
        }
    }
    catch (std::exception e)
    {
        LOG_ERROR(e.what());
        throw;
    }
}

void SchedulerInterface::HapticScheduler::finalize()
{
    LOG_INFO("SchedulerInterface::HapticScheduler::finalize");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void SchedulerInterface::onConfigure(const std::string &configFile)
{
    LOG_INFO("SchedulerInterface::onConfigure");

    auto config = JSON::Object::fromFile(configFile).getItem<JSON::Object>("Scheduler");

    if (auto &item = config.getItem("ForceDecodersSynchro"))
    {
        m_masterClock.setForceDecodersSynchro(item.as<bool>());
    }
    
    m_audioScheduler.setInterface(m_audioInterface);
    m_audioScheduler.setLatency(std::chrono::milliseconds{config.getItem("Latency").as<int>()});

    m_videoScheduler.setInterface(m_videoInterface);
    m_videoScheduler.setJitter(std::chrono::milliseconds{config.getItem("Jitter").as<int>()});
 
    m_hapticScheduler.setInterface(m_hapticInterface);
    m_hapticScheduler.setLatency(std::chrono::milliseconds{config.getItem("Latency").as<int>()});
}

void SchedulerInterface::onStartEvent()
{
    LOG_INFO("SchedulerInterface::onStartEvent");

    m_audioScheduler.getInput().open();
    m_audioScheduler.start();

    m_videoScheduler.getInput().open();
    m_videoScheduler.start();

    m_hapticScheduler.getInput().open();
    m_hapticScheduler.start();
}

void SchedulerInterface::onStopEvent()
{
    m_audioScheduler.getInput().close();
    m_audioScheduler.stop();
    m_audioScheduler.getInput().clear();

    m_videoScheduler.getInput().close();
    m_videoScheduler.stop();
    m_videoScheduler.getInput().clear();

    m_hapticScheduler.getInput().close();
    m_hapticScheduler.stop();
    m_hapticScheduler.getInput().clear();

    LOG_INFO("SchedulerInterface::onStopEvent");
}
