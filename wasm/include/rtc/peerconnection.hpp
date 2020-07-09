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

#ifndef RTC_PEERCONNECTION_H
#define RTC_PEERCONNECTION_H

#include "candidate.hpp"
#include "configuration.hpp"
#include "datachannel.hpp"
#include "description.hpp"
#include "include.hpp"

#include <functional>
#include <optional>
#include <variant>

namespace rtc {

class PeerConnection final {
public:
	PeerConnection();
	PeerConnection(const Configuration &config);
	~PeerConnection();

	std::shared_ptr<DataChannel> createDataChannel(const string &label);

	void setRemoteDescription(const Description &description);
	void addRemoteCandidate(const Candidate &candidate);

	void onDataChannel(std::function<void(std::shared_ptr<DataChannel>)> callback);
	void onLocalDescription(std::function<void(const Description &description)> callback);
	void onLocalCandidate(std::function<void(const Candidate &candidate)> callback);

protected:
	void triggerDataChannel(std::shared_ptr<DataChannel> dataChannel);
	void triggerLocalDescription(const Description &description);
	void triggerLocalCandidate(const Candidate &candidate);

	std::function<void(std::shared_ptr<DataChannel>)> mDataChannelCallback;
	std::function<void(const Description &description)> mLocalDescriptionCallback;
	std::function<void(const Candidate &candidate)> mLocalCandidateCallback;

private:
	int mId;

	static void DataChannelCallback(int dc, void *ptr);
	static void DescriptionCallback(const char *sdp, const char *type, void *ptr);
	static void CandidateCallback(const char *candidate, const char *mid, void *ptr);
};

} // namespace rtc

#endif // RTC_PEERCONNECTION_H
