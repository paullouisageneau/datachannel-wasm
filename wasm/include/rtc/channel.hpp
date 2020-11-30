/*************************************************************************
 *   Copyright (C) 2017-2020 by Paul-Louis Ageneau                       *
 *   paul-louis (at) ageneau (dot) org                                   *
 *                                                                       *
 *   This file is part of datachannel-wasm.                              *
 *                                                                       *
 *   datachannel-wasm is free software: you can redistribute it and/or   *
 *   modify it under the terms of the GNU Affero General Public License  *
 *   as published by the Free Software Foundation, either version 3 of   *
 *   the License, or (at your option) any later version.                 *
 *                                                                       *
 *   datachannel-wasm is distributed in the hope that it will be useful, *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the        *
 *   GNU Affero General Public License for more details.                 *
 *                                                                       *
 *   You should have received a copy of the GNU Affero General Public    *
 *   License along with datachannel-wasm.                                *
 *   If not, see <http://www.gnu.org/licenses/>.                         *
 *************************************************************************/

#ifndef RTC_CHANNEL_H
#define RTC_CHANNEL_H

#include "include.hpp"

namespace rtc {

class Channel {
public:
	virtual ~Channel() = default;

	virtual void close() = 0;
	virtual bool send(message_variant data) = 0;
	virtual bool send(const byte *data, size_t size) = 0;

	virtual bool isOpen() const = 0;
	virtual bool isClosed() const = 0;
	virtual size_t bufferedAmount() const;

	void onOpen(std::function<void()> callback);
	void onClosed(std::function<void()> callback);
	void onError(std::function<void(string error)> callback);
	void onMessage(std::function<void(message_variant data)> callback);
	void onMessage(std::function<void(binary data)> binaryCallback,
	               std::function<void(string data)> stringCallback);
	void onBufferedAmountLow(std::function<void()> callback);

	virtual void setBufferedAmountLowThreshold(size_t amount);

protected:
	virtual void triggerOpen();
	virtual void triggerClosed();
	virtual void triggerError(string error);
	virtual void triggerMessage(message_variant data);
	virtual void triggerBufferedAmountLow();

private:
	std::function<void()> mOpenCallback;
	std::function<void()> mClosedCallback;
	std::function<void(string error)> mErrorCallback;
	std::function<void(message_variant data)> mMessageCallback;
	std::function<void()> mBufferedAmountLowCallback;
};

} // namespace rtc

#endif // RTC_CHANNEL_H
