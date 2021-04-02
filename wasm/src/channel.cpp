/**
 * Copyright (c) 2017-2020 Paul-Louis Ageneau
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "channel.hpp"

namespace rtc {

using std::function;

size_t Channel::bufferedAmount() const { return 0; /* Dummy */ }

void Channel::onOpen(std::function<void()> callback) { mOpenCallback = std::move(callback); }

void Channel::onClosed(std::function<void()> callback) { mClosedCallback = std::move(callback); }

void Channel::onError(std::function<void(string)> callback) {
	mErrorCallback = std::move(callback);
}

void Channel::onMessage(std::function<void(message_variant data)> callback) {
	mMessageCallback = std::move(callback);
}

void Channel::onMessage(std::function<void(binary data)> binaryCallback,
                        std::function<void(string data)> stringCallback) {
	onMessage([binaryCallback = std::move(binaryCallback),
	           stringCallback = std::move(stringCallback)](message_variant data) {
		std::visit(overloaded{binaryCallback, stringCallback}, std::move(data));
	});
}

void Channel::onBufferedAmountLow(std::function<void()> callback) {
	mBufferedAmountLowCallback = std::move(callback);
}

void Channel::setBufferedAmountLowThreshold(size_t amount) { /* Dummy */
}

void Channel::triggerOpen() {
	if (mOpenCallback)
		mOpenCallback();
}

void Channel::triggerClosed() {
	if (mClosedCallback)
		mClosedCallback();
}

void Channel::triggerError(string error) {
	if (mErrorCallback)
		mErrorCallback(std::move(error));
}

void Channel::triggerMessage(const message_variant data) {
	if (mMessageCallback)
		mMessageCallback(data);
}

void Channel::triggerBufferedAmountLow() {
	if(mBufferedAmountLowCallback)
		mBufferedAmountLowCallback();
}

} // namespace rtc
