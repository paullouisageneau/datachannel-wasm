/*************************************************************************
 *   Copyright (C) 2017-2020 by Paul-Louis Ageneau                       *
 *   paul-louis (at) ageneau (dot) org                                   *
 *                                                                       *
 *   This file is part of datachannels.                                  *
 *                                                                       *
 *   datachannels is free software: you can redistribute it and/or       *
 *   modify it under the terms of the GNU Affero General Public License  *
 *   as published by the Free Software Foundation, either version 3 of   *
 *   the License, or (at your option) any later version.                 *
 *                                                                       *
 *   datachannels is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the        *
 *   GNU Affero General Public License for more details.                 *
 *                                                                       *
 *   You should have received a copy of the GNU Affero General Public    *
 *   License along with datachannels.                                    *
 *   If not, see <http://www.gnu.org/licenses/>.                         *
 *************************************************************************/

#ifndef RTC_WEBSOCKET_H
#define RTC_WEBSOCKET_H

#include "channel.hpp"

#include <variant>

namespace rtc {

// WebSocket wrapper for emscripten
class WebSocket : public Channel {
public:
	WebSocket(void);
	WebSocket(const string &url);
	~WebSocket(void);

	void open(const string &url);
	void close(void);
	void send(const std::variant<binary, string> &data);

	bool isOpen(void) const;
	bool isClosed(void) const;

private:
	void triggerOpen(void);

	int mId;
	bool mConnected;

	static void OpenCallback(void *ptr);
	static void ErrorCallback(const char *error, void *ptr);
	static void MessageCallback(const char *data, int size, void *ptr);
};

} // namespace rtc

#endif // RTC_WEBSOCKET_H
