/*
* Copyright (c) 2024 InterDigital
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/


#include <streaming/client_dash.h>
#include <iloj/misc/filesystem.h>
#include <iloj/misc/logger.h>
#include <iloj/misc/dll.h>

using namespace iloj::misc; 

bool DashSegmentReceiver::loadDll()
{
    LoadProcEx("v3c_dash_streamer", "dashc_createSegmentProvider", createSegmentProvider);
    LoadProcEx("v3c_dash_streamer", "dashc_getChunk", getChunk);
    LoadProcEx("v3c_dash_streamer", "dashc_initSegmentProvider", initSegmentProvider);
    LoadProcEx("v3c_dash_streamer", "dashc_startStreaming", startStreaming);
    LoadProcEx("v3c_dash_streamer", "dashc_stopStreaming", stopStreaming);
    LoadProcEx("v3c_dash_streamer", "dashc_getNetworkStatus", getNetworkStatus);
    LoadProcEx("v3c_dash_streamer", "dashc_getHTTPStatus", getHTTPStatus);

    if ((createSegmentProvider != nullptr) && (getChunk != nullptr) &&
        (initSegmentProvider != nullptr) && (startStreaming != nullptr) &&
        (stopStreaming != nullptr) && (getNetworkStatus != nullptr) &&
        (getHTTPStatus != nullptr))
    {
        LOG_INFO("V3C DASH Streamer loaded");
        m_dllLoaded = true;
    }
    else
    { 
        m_dllLoaded = false;
    }

    return m_dllLoaded;
}

bool DashSegmentReceiver::dashReceiverStarted()
{
    return m_dashReceiverStarted;
}

Chunk DashSegmentReceiver::getMediaChunk()
{
    return convertToChunk(getChunk());
}

Chunk DashSegmentReceiver::convertToChunk(DashChunk_t *dashChunk)
{
    Chunk chunk;

    chunk.setData(std::move(dashChunk->data));
    chunk.getHeader().setNumberOfFrames(dashChunk->sampleCount);
    chunk.getHeader().setTypeId(dashChunk->typeId);
    chunk.getHeader().setDuration(std::chrono::duration<double>(dashChunk->frameDuration));
    chunk.getHeader().setErrorStreamer(dashChunk->errorStreamer);
    chunk.getHeader().setSeqNumber(dashChunk->seqNumber);
    chunk.getHeader().setSegmentDuration(dashChunk->segmentDuration);
    chunk.getHeader().setTimestampDbg(dashChunk->timestamp);

    if (dashChunk->errorStreamer != 0)
    {
        DASHCcode keyD = static_cast<DASHCcode>(dashChunk->errorStreamer);
        CURLcode keyC = static_cast<CURLcode>(getNetworkStatus());

        std::string logMsgD = dashCodeMap.at(keyD);
        std::string logMsgC = curlCodeMap.at(keyC);

        LOG_ERROR("RunTime Streamer Error = ", logMsgD);
        LOG_ERROR("Network status, ", logMsgC);
        LOG_ERROR("HTTP status, ", getHTTPStatus());

        // by default error handling is a stop of the app
        // but in case of bad network conditions, the level is set to 0
        // to give a chance to retrieve better conditions.
        int level = 2;
        if ((keyD == DASHCcode::DASHCLIENT_ACCESS_TO_SEGMENT_FAILED)
            &&(keyC == CURLcode::CURL_COULDNT_CONNECT)||(keyC == CURLcode::CURL_OPERATION_TIMEDOUT))
        {
            level=0;
        }

        if (m_decoderInterface)
            m_decoderInterface->onErrorEvent(level, (unsigned int)dashChunk->errorStreamer);
    }

    return chunk;
}


CURLcode DashSegmentReceiver::getDashNetworkStatus()
{
    return  static_cast<CURLcode>(getNetworkStatus());
}

long DashSegmentReceiver::getDashHTTPStatus()
{
    return getDashHTTPStatus();
}

void DashSegmentReceiver::onStart()
{
    createSegmentProvider();

    m_dashReceiverStarted = false;
}


void DashSegmentReceiver::initialize()
{
    DashError errMsg;
    int err = initSegmentProvider(m_mpdURL.data(), &errMsg);
    std::string logMsgD{ errMsg.message };
    DASHCcode keyD = static_cast<DASHCcode>(errMsg.code);
    CURLcode keyC = static_cast<CURLcode>(getNetworkStatus());

    if (err != 0)
    {
        logMsgD += ": ";
        logMsgD += dashCodeMap.at(keyD);
        std::string logMsgC = curlCodeMap.at(keyC);

        LOG_ERROR("ClientInterface init ", logMsgD);
        LOG_ERROR("Network status, ", logMsgC);
        LOG_ERROR("HTTP status, ", getHTTPStatus());

        if (m_decoderInterface)
            m_decoderInterface->onErrorEvent(2, (unsigned int)errMsg.code);

        finish();
        return;
    }

    // for HJIF fallback solution only
    if ((keyD == DASHCcode::DASHCLIENT_HJIF_NOT_FOUND)
        || (keyD == DASHCcode::DASHCLIENT_ACCESS_TO_HJIF_FAILED))
    {
        logMsgD += ": ";
        logMsgD += dashCodeMap.at(keyD);
        std::string logMsgC = curlCodeMap.at(keyC);

        LOG_INFO("ClientInterface hjif ", logMsgD);
        LOG_INFO("Network status, ", logMsgC);
        LOG_INFO("HTTP status, ", getHTTPStatus());
    }
    // end of HJIF fallback

    if (static_cast<DASHCcode>(startStreaming()) != DASHCcode::DASHCLIENT_OK)
    {
        LOG_ERROR("ClientInterface cannot start streamer");

        if (m_decoderInterface)
            m_decoderInterface->onErrorEvent(2, (unsigned int)DASHCcode::DASHCLIENT_STREAMER_ERROR);

        finish();
        return;
    }
    
    // startStreaming() has returned successfully, this ensures that the circular buffer is full, and
    // chunks can be pushed to the decoder.
    m_dashReceiverStarted = true;
}

void DashSegmentReceiver::idle()
{
}

void DashSegmentReceiver::finalize()
{
    stopStreaming();
}

void DashSegmentReceiver::onStop()
{
    m_dashReceiverStarted = false;
}
