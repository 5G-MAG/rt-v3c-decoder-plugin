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

#include <common/misc/spsc_queue.h>
#include <common/stream/item.h>
#include <decoder/decoder.h>
#include <iloj/misc/json.h>
#include <iloj/misc/socket.h>
#include <iloj/misc/thread.h>
#include <iloj/misc/time.h>
#include <interface/client.h>
#include <string>
#include <chrono>

typedef std::chrono::steady_clock::time_point timePoint;


// DASH Client error code
enum class DASHCcode
{
    DASHCLIENT_OK = 0,            // no error occurred
    DASHCLIENT_INIT_FAILED,
    DASHCLIENT_INIT_SEGMENT_EMPTY,
    DASHCLIENT_ACCESS_TO_MPD_FAILED,
    DASHCLIENT_PARSE_MPD_FAILED,
    DASHCLIENT_MPD_NOT_FOUND,
    DASHCLIENT_CURL_CONNECTION_ERROR,
    DASHCLIENT_HTTP_ERROR,
    DASHCLIENT_STREAMER_ERROR,
    DASHCLIENT_EXTRACTION_ERROR,
    DASHCLIENT_HJIF_ERROR,
    DASHCLIENT_HJIF_NOT_FOUND,
    DASHCLIENT_ACCESS_TO_HJIF_FAILED,
    DASHCLIENT_CONFIG_NOT_FOUND,
    DASHCLIENT_NO_HAPTIC,
    DASHCLIENT_INITSEGMENT_NOT_FOUND,
    DASHCLIENT_SEGMENT_NOT_FOUND,
    DASHCLIENT_ACCESS_TO_SEGMENT_FAILED,
    DASHCLIENT_UNKNOWN
};

const std::map<DASHCcode, std::string> dashCodeMap{
    {DASHCcode::DASHCLIENT_OK,                       "no error"},
    {DASHCcode::DASHCLIENT_INIT_FAILED,              "initialisation failed"},
    {DASHCcode::DASHCLIENT_INIT_SEGMENT_EMPTY,       "the init segment is empty"},
    {DASHCcode::DASHCLIENT_ACCESS_TO_MPD_FAILED,     "cannot access to the MPD file"},
    {DASHCcode::DASHCLIENT_PARSE_MPD_FAILED,         "parsing MPD file failed"},
    {DASHCcode::DASHCLIENT_MPD_NOT_FOUND,            "MPD file not found"},
    {DASHCcode::DASHCLIENT_CURL_CONNECTION_ERROR,    "CURL connection error"},
    {DASHCcode::DASHCLIENT_HTTP_ERROR,               "HTTP error"},
    {DASHCcode::DASHCLIENT_STREAMER_ERROR,           "streamer error"},
    {DASHCcode::DASHCLIENT_EXTRACTION_ERROR,         "extraction error"},
    {DASHCcode::DASHCLIENT_HJIF_ERROR,               "HJIF error"},
    {DASHCcode::DASHCLIENT_HJIF_NOT_FOUND,           "HJIF file not found"},
    {DASHCcode::DASHCLIENT_ACCESS_TO_HJIF_FAILED,    "access to hjif file failed"},
    {DASHCcode::DASHCLIENT_CONFIG_NOT_FOUND,         "config file for haptic not found"},
    {DASHCcode::DASHCLIENT_NO_HAPTIC,                "no haptic"},
    {DASHCcode::DASHCLIENT_INITSEGMENT_NOT_FOUND,    "init segment not found"},
    {DASHCcode::DASHCLIENT_SEGMENT_NOT_FOUND,        "segment not found"},
    {DASHCcode::DASHCLIENT_ACCESS_TO_SEGMENT_FAILED, "access to segment failed"},
    {DASHCcode::DASHCLIENT_UNKNOWN,                  "error unknown"}
};

// CURL error code - limited list, only an excerpt of the official CURL error code
enum class CURLcode
{
    CURL_OK = 0,            // no error occurred
    CURL_UNSUPPORTED_PROTOCOL = 1,
    CURL_FAILED_INIT = 2,
    CURL_URL_MALFORMAT = 3,
    CURL_COULDNT_RESOLVE_PROXY = 5,
    CURL_COULDNT_RESOLVE_HOST = 6,
    CURL_COULDNT_CONNECT = 7,
    CURL_WEIRD_SERVER_REPLY = 8,
    CURL_REMOTE_ACCESS_DENIED = 9,
    CURL_HTTP_RETURNED_ERROR = 22,
    CURL_OPERATION_TIMEDOUT = 28,
    CURL_INTERFACE_FAILED = 45,
    CURL_UNKNOWN_OPTION = 48,
    CURL_RECV_ERROR = 56,
    CURL_AUTH_ERROR = 94,
    CURL_UNKNOWN
};

const std::map<CURLcode, std::string> curlCodeMap{
    {CURLcode::CURL_OK,                    "no error"},
    {CURLcode::CURL_UNSUPPORTED_PROTOCOL,  "protocol not supported by libcurl"},
    {CURLcode::CURL_FAILED_INIT,           "Early initialization code failed"},
    {CURLcode::CURL_URL_MALFORMAT,         "The URL was not properly formatted"},
    {CURLcode::CURL_COULDNT_RESOLVE_PROXY, "Could not resolve proxy"},
    {CURLcode::CURL_COULDNT_RESOLVE_HOST,  "Could not resolve host"},
    {CURLcode::CURL_COULDNT_CONNECT,       "Failed to connect() to host or proxy"},
    {CURLcode::CURL_WEIRD_SERVER_REPLY,    "The server sent data libcurl could not parse"},
    {CURLcode::CURL_REMOTE_ACCESS_DENIED,  "denied access to the resource given in the URL"},
    {CURLcode::CURL_HTTP_RETURNED_ERROR,   "HTTP server returns an error code that is >= 400"},
    {CURLcode::CURL_OPERATION_TIMEDOUT,    "Operation timeout"},
    {CURLcode::CURL_INTERFACE_FAILED,      "Interface error"},
    {CURLcode::CURL_UNKNOWN_OPTION,        "An option passed to libcurl is not recognized/known"},
    {CURLcode::CURL_RECV_ERROR,            "Failure with receiving network data"},
    {CURLcode::CURL_AUTH_ERROR,            "An authentication function returned an error."},
    {CURLcode::CURL_UNKNOWN,               "unknown error"}
};

struct DashError {
    unsigned int code;
    char message[256];
};

// DASH Client chunk structure
// to be constructed as a union??
struct DashChunk_t
{
    // Header
    size_t ssvh{ 0 };
    unsigned int sampleCount{ 0 };
    unsigned char typeId{ 0 };
    double frameDuration{ 0.0 };
    unsigned int errorStreamer{ 0 };
    unsigned int seqNumber{ 0 };
    long long timestamp{ 0 };
    double segmentDuration{ 0.0 };

    // data
    std::vector<unsigned char> data;
};



/// <summary>
/// Service dedicated to downloading the MediaChunks made available by V3C Dash Streamer
/// and feeding them to the Client as Chunks.
/// This object has the single Producer of the Chunks consumed by the Client's Consumer.
///
/// The running cycle of the SegmentReceiver is controlled by the Client, which is its only user.
/// </summary>
class DashSegmentReceiver : public iloj::misc::Service
{
public:
    /// <summary>
    /// full URL of the mpd file of the current media
    /// used to open the dash client
    /// </summary>
    std::string m_mpdURL;
    bool m_dllLoaded{ false };
    bool m_dashReceiverStarted{ false };

#ifdef MEASUREMENT_LOG
    long long m_previous_tp{ 0 };
#endif
    /// <summary>
    ///  types and function pointers used for explicit dll
    /// </summary>
    using CreateSegmentProvider = void(void);
    CreateSegmentProvider* createSegmentProvider = nullptr;
    using GetChunk = DashChunk_t * (void);
    GetChunk* getChunk = nullptr;
    using InitSegmentProvider = int(std::string url, DashError*);
    using StartStreaming = int(void);
    InitSegmentProvider* initSegmentProvider = nullptr;
    StartStreaming* startStreaming = nullptr;
    using StopStreaming = void(void);
    StopStreaming* stopStreaming = nullptr;
    using GetNetworkStatus = uint32_t(void);
    GetNetworkStatus* getNetworkStatus = nullptr;
    using GetHTTPStatus = long(void);
    GetHTTPStatus* getHTTPStatus = nullptr;

private:

    DecoderInterface* m_decoderInterface;

    /// <summary>
    /// Service interface
    /// </summary>
    void onStart() override;
    void initialize() override;
    void idle() override;
    void finalize() override;
    void onStop() override;

public:

    /// <summary>
    /// Create a Chunk from the data of a MediaChunk
    /// </summary>
    /// <param name="mediaChunk">a MediaChunk to move data from</param>
    /// <returns>a Chunk containing the data of the MediaChunk</returns>
    Chunk convertToChunk(DashChunk_t* dashChunk);

    /// <summary>
    /// return the Network status
    /// </summary>
    /// <returns>a CURLcode containing the status of the network</returns>
    CURLcode getDashNetworkStatus();

    /// <summary>
    /// return the HTTP status
    /// </summary>
    /// <returns>a long value telling the HTTP status, e.g. error 404</returns>
    long getDashHTTPStatus();

    /// <summary>
    /// return whether the DASH receiver has started
    /// </summary>
    /// <returns>a boolean value</returns>
    bool dashReceiverStarted();

    /// <summary>
    /// set the decoder interface pointer
    /// </summary>
    /// <param name="decoderInterface">set the decoder interface pointer, needed for the event callback</param>
    void setDecoderInterface(DecoderInterface* decoderInterface) { m_decoderInterface = decoderInterface; }

    /// <summary>
    /// Load the dll.
    /// </summary>
    /// <returns>true or false depending on the dll availability</returns>
    bool loadDll();

    /// <summary>
    /// Get the media chunk that can be decoded.
    /// </summary>
    /// <returns>Chunk</returns>
    Chunk getMediaChunk();

    // NB: Chunk defined in common/stream/chunk.h
};
