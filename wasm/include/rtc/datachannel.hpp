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

#ifndef RTC_DATACHANNEL_H
#define RTC_DATACHANNEL_H

#include "channel.hpp"
#include "include.hpp"

namespace rtc {

class DataChannel final : public Channel {
public:
	explicit DataChannel(int id);
	~DataChannel();

	void close() override;
	bool send(message_variant data) override;
	bool send(const byte *data, size_t size) override;

	bool isOpen() const override;
	bool isClosed() const override;

	string label() const;

private:
	int mId;
	string mLabel;
	bool mConnected;

	static void OpenCallback(void *ptr);
	static void ErrorCallback(const char *error, void *ptr);
	static void MessageCallback(const char *data, int size, void *ptr);
};

} // namespace rtc

#endif // RTC_DATACHANNEL_H
