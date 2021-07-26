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

#ifndef RTC_CONFIGURATION_H
#define RTC_CONFIGURATION_H

#include "common.hpp"

#include <vector>

namespace rtc {

struct IceServer {
	enum class Type : int { Stun = 0, Turn, Dummy };
	enum class RelayType : int { TurnUdp = 0, TurnTcp, TurnTls };

	// Note: Contrary to libdatachannel, the URL constructor does not parse the URL.
	// Instead, it creates a Dummy IceServer to pass the URL as-is to the browser.
	IceServer(const string &url);

	// STUN
	IceServer(string hostname_, uint16_t port_);
	IceServer(string hostname_, string service_);

	// TURN
	IceServer(string hostname_, uint16_t port, string username_, string password_,
	          RelayType relayType_ = RelayType::TurnUdp);
	IceServer(string hostname_, string service_, string username_, string password_,
	          RelayType relayType_ = RelayType::TurnUdp);

	string hostname;
	uint16_t port;
	Type type;
	string username;
	string password;
	RelayType relayType;
};

struct Configuration {
	std::vector<IceServer> iceServers;
};

} // namespace rtc

#endif // RTC_CONFIGURATION_H
