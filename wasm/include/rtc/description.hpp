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

#ifndef RTC_DESCRIPTION_H
#define RTC_DESCRIPTION_H

#include "include.hpp"

#include <iostream>

namespace rtc {

class Description {
public:
	Description(const string &sdp, const string &type) : mSdp(sdp), mType(type) {}
	string typeString() const { return mType; }
	operator string() const { return mSdp; }

private:
	string mSdp;
	string mType;
};

} // namespace rtc

std::ostream &operator<<(std::ostream &out, const rtc::Description &description);

#endif // RTC_DESCRIPTION_H
