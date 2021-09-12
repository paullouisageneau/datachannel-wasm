/**
 * Copyright (c) 2017-2021 Paul-Louis Ageneau
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

#include "description.hpp"

#include <unordered_map>

namespace rtc {

Description::Description(const string &sdp, Type type) : mSdp(sdp), mType(typeToString(type)) {}

Description::Description(const string &sdp, string typeString)
    : mSdp(sdp), mType(std::move(typeString)) {}

Description::Type Description::type() const { return stringToType(mType); }

string Description::typeString() const { return mType; }

Description::operator string() const { return mSdp; }

Description::Type Description::stringToType(const string &typeString) {
	using TypeMap_t = std::unordered_map<string, Type>;
	static const TypeMap_t TypeMap = {{"unspec", Type::Unspec},
	                                  {"offer", Type::Offer},
	                                  {"answer", Type::Answer},
	                                  {"pranswer", Type::Pranswer},
	                                  {"rollback", Type::Rollback}};
	auto it = TypeMap.find(typeString);
	return it != TypeMap.end() ? it->second : Type::Unspec;
}

string Description::typeToString(Type type) {
	switch (type) {
	case Type::Unspec:
		return "unspec";
	case Type::Offer:
		return "offer";
	case Type::Answer:
		return "answer";
	case Type::Pranswer:
		return "pranswer";
	case Type::Rollback:
		return "rollback";
	default:
		return "unknown";
	}
}

} // namespace rtc

std::ostream &operator<<(std::ostream &out, const rtc::Description &description) {
	return out << std::string(description);
}

std::ostream &operator<<(std::ostream &out, rtc::Description::Type type) {
	return out << rtc::Description::typeToString(type);
}
