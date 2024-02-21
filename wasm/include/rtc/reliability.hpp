/**
 * Copyright (c) 2017-2022 Paul-Louis Ageneau
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef RTC_RELIABILITY_H
#define RTC_RELIABILITY_H

#include "common.hpp"

#include <chrono>

namespace rtc {

struct Reliability {
	// It true, the channel does not enforce message ordering and out-of-order delivery is allowed
	bool unordered = false;

	// If both maxPacketLifeTime or maxRetransmits are unset, the channel is reliable.
	// If either maxPacketLifeTime or maxRetransmits is set, the channel is unreliable.
	// (The settings are exclusive so both maxPacketLifetime and maxRetransmits must not be set.)

	// Time window during which transmissions and retransmissions may occur
	optional<std::chrono::milliseconds> maxPacketLifeTime;

	// Maximum number of retransmissions that are attempted
	optional<unsigned int> maxRetransmits;
};

} // namespace rtc

#endif // RTC_RELIABILITY_H
