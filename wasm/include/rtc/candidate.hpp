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

#ifndef RTC_CANDIDATE_H
#define RTC_CANDIDATE_H

#include "include.hpp"

#include <iostream>

namespace rtc {

class Candidate {
public:
	Candidate(const string &candidate, const string &mid) : mCandidate(candidate), mMid(mid) {}
	string candidate() const { return mCandidate; }
	string mid() const { return mMid; }
	operator string() const { return "a=" + mCandidate; }

private:
	string mCandidate;
	string mMid;
};

} // namespace rtc

std::ostream &operator<<(std::ostream &out, const rtc::Candidate &candidate);

#endif // RTC_CANDIDATE_H
