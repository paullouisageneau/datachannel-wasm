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

#include <functional>
#include <variant>

namespace rtc {

class Channel {
public:
	virtual ~Channel(void) = default;

	virtual void close(void) = 0;
	virtual void send(const std::variant<binary, string> &data) = 0;

	virtual bool isOpen(void) const = 0;
	virtual bool isClosed(void) const = 0;

	void onOpen(std::function<void()> callback);
	void onClosed(std::function<void()> callback);
	void onError(std::function<void(const string &error)> callback);
	void onMessage(std::function<void(const std::variant<binary, string> &data)> callback);
	void onMessage(std::function<void(const binary &data)> binaryCallback,
	               std::function<void(const string &data)> stringCallback);

protected:
	virtual void triggerOpen(void);
	virtual void triggerClosed(void);
	virtual void triggerError(const string &error);
	virtual void triggerMessage(const std::variant<binary, string> &data);

private:
	std::function<void()> mOpenCallback;
	std::function<void()> mClosedCallback;
	std::function<void(const string &error)> mErrorCallback;
	std::function<void(const std::variant<binary, string> &data)> mMessageCallback;
};

} // namespace rtc

#endif // RTC_CHANNEL_H
