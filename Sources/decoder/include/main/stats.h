/*
* License: 5G-MAG Public License (v1.0)
* Author: Jordi J. Gimenez (5G-MAG)
* Copyright: (C) 2026 5G-MAG Association
*
* For full license terms please see the LICENSE file distributed with this
* program. If this file is missing then the license can be retrieved from
* https://www.5g-mag.com/license.
*/

#pragma once

#include <atomic>

// Counters the player can read while a title plays (GetDecoderStats). They only grow; a reader
// that wants a rate subtracts two readings.
struct DecoderStats
{
    std::atomic<unsigned long long> framesPresented{0}; // video frames handed to the renderer
    std::atomic<unsigned long long> framesLate{0};      // video frames that arrived after their presentation time
    std::atomic<unsigned long long> lastLateMs{0};      // lateness of the last late frame
    std::atomic<unsigned long long> chunks{0};          // media chunks (DASH segments) received
    std::atomic<unsigned long long> bytes{0};           // payload bytes of those chunks
};

inline DecoderStats &decoderStats()
{
    static DecoderStats stats;
    return stats;
}
