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

#ifndef RTC_DATACHANNEL_H
#define RTC_DATACHANNEL_H

#include "channel.hpp"
#include "common.hpp"

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
	size_t bufferedAmount() const override;
	string label() const;

	void setBufferedAmountLowThreshold(size_t amount) override;

private:
	int mId;
	string mLabel;
	bool mConnected;

	static void OpenCallback(void *ptr);
	static void ErrorCallback(const char *error, void *ptr);
	static void MessageCallback(const char *data, int size, void *ptr);
	static void BufferedAmountLowCallback(void *ptr);
};

} // namespace rtc

#endif // RTC_DATACHANNEL_H
