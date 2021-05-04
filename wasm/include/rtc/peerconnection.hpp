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

#ifndef RTC_PEERCONNECTION_H
#define RTC_PEERCONNECTION_H

#include "candidate.hpp"
#include "configuration.hpp"
#include "datachannel.hpp"
#include "description.hpp"
#include "common.hpp"
#include "reliability.hpp"

#include <functional>
#include <optional>
#include <variant>

namespace rtc {

struct DataChannelInit {
	Reliability reliability = {};
};

class PeerConnection final {
public:
	enum class State : int {
		New = 0,
		Connecting = 1,
		Connected = 2,
		Disconnected = 3,
		Failed = 4,
		Closed = 5
	};

	enum class GatheringState : int {
		New = 0,
		InProgress = 1,
		Complete = 2
	};

	enum class SignalingState : int {
		Stable = 0,
		HaveLocalOffer = 1,
		HaveRemoteOffer = 2,
		HaveLocalPranswer = 3,
		HaveRemotePranswer = 4,
	} rtcSignalingState;

	PeerConnection();
	PeerConnection(const Configuration &config);
	~PeerConnection();

	std::optional<Description> localDescription() const;

	std::shared_ptr<DataChannel> createDataChannel(const string &label, DataChannelInit init = {});

	void setRemoteDescription(const Description &description);
	void addRemoteCandidate(const Candidate &candidate);

	void onDataChannel(std::function<void(std::shared_ptr<DataChannel>)> callback);
	void onLocalDescription(std::function<void(const Description &description)> callback);
	void onLocalCandidate(std::function<void(const Candidate &candidate)> callback);
	void onStateChange(std::function<void(State state)> callback);
	void onGatheringStateChange(std::function<void(GatheringState state)> callback);
	void onSignalingStateChange(std::function<void(SignalingState state)> callback);

protected:
	void triggerDataChannel(std::shared_ptr<DataChannel> dataChannel);
	void triggerLocalDescription(const Description &description);
	void triggerLocalCandidate(const Candidate &candidate);
	void triggerStateChange(State state);
	void triggerGatheringStateChange(GatheringState state);
	void triggerSignalingStateChange(SignalingState state);

	std::function<void(std::shared_ptr<DataChannel>)> mDataChannelCallback;
	std::function<void(const Description &description)> mLocalDescriptionCallback;
	std::function<void(const Candidate &candidate)> mLocalCandidateCallback;
	std::function<void(State candidate)> mStateChangeCallback;
	std::function<void(GatheringState candidate)> mGatheringStateChangeCallback;
	std::function<void(SignalingState candidate)> mSignalingStateChangeCallback;

private:
	int mId;

	static void DataChannelCallback(int dc, void *ptr);
	static void DescriptionCallback(const char *sdp, const char *type, void *ptr);
	static void CandidateCallback(const char *candidate, const char *mid, void *ptr);
	static void StateChangeCallback(int state, void *ptr);
	static void GatheringStateChangeCallback(int state, void *ptr);
	static void SignalingStateChangeCallback(int state, void *ptr);
};

} // namespace rtc

#endif // RTC_PEERCONNECTION_H
