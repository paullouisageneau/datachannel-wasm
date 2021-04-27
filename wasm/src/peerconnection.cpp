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

#include "peerconnection.hpp"

#include <emscripten/emscripten.h>

#include <exception>
#include <iostream>
#include <stdexcept>

extern "C" {
extern int rtcCreatePeerConnection(const char **iceServers);
extern void rtcDeletePeerConnection(int pc);
extern char *rtcLocalDescriptionSdp(int pc);
extern char *rtcLocalDescriptionType(int pc);
extern int rtcCreateDataChannel(int pc, const char *label);
extern void rtcSetDataChannelCallback(int pc, void (*dataChannelCallback)(int, void *));
extern void rtcSetLocalDescriptionCallback(int pc,
                                           void (*descriptionCallback)(const char *, const char *,
                                                                       void *));
extern void rtcSetLocalCandidateCallback(int pc, void (*candidateCallback)(const char *,
                                                                           const char *, void *));
extern void rtcSetStateChangeCallback(int pc, void (*stateChangeCallback)(int, void *));
extern void rtcSetGatheringStateChangeCallback(int pc, void (*gatheringStateChangeCallback)(int, void *));
extern void rtcSetSignalingStateChangeCallback(int pc, void (*signalingStateChangeCallback)(int, void *));
extern void rtcSetRemoteDescription(int pc, const char *sdp, const char *type);
extern void rtcAddRemoteCandidate(int pc, const char *candidate, const char *mid);
extern void rtcSetUserPointer(int i, void *ptr);
}

namespace rtc {

using std::function;
using std::optional;
using std::shared_ptr;
using std::vector;

void PeerConnection::DataChannelCallback(int dc, void *ptr) {
	PeerConnection *p = static_cast<PeerConnection *>(ptr);
	if (p)
		p->triggerDataChannel(std::make_shared<DataChannel>(dc));
}

void PeerConnection::DescriptionCallback(const char *sdp, const char *type, void *ptr) {
	PeerConnection *p = static_cast<PeerConnection *>(ptr);
	if (p)
		p->triggerLocalDescription(Description(sdp, type));
}

void PeerConnection::CandidateCallback(const char *candidate, const char *mid, void *ptr) {
	PeerConnection *p = static_cast<PeerConnection *>(ptr);
	if (p)
		p->triggerLocalCandidate(Candidate(candidate, mid));
}

void PeerConnection::StateChangeCallback(int state, void *ptr) {
	PeerConnection *p = static_cast<PeerConnection *>(ptr);
	if (p)
		p->triggerStateChange(static_cast<State>(state));
}

void PeerConnection::GatheringStateChangeCallback(int state, void *ptr) {
	PeerConnection *p = static_cast<PeerConnection *>(ptr);
	if (p)
		p->triggerGatheringStateChange(static_cast<GatheringState>(state));
}

void PeerConnection::SignalingStateChangeCallback(int state, void *ptr) {
	PeerConnection *p = static_cast<PeerConnection *>(ptr);
	if (p)
		p->triggerSignalingStateChange(static_cast<SignalingState>(state));
}

PeerConnection::PeerConnection(const Configuration &config) {
	vector<const char *> ptrs;
	ptrs.reserve(config.iceServers.size());
	for (const string &s : config.iceServers)
		ptrs.push_back(s.c_str());
	ptrs.push_back(nullptr);
	mId = rtcCreatePeerConnection(ptrs.data());
	if (!mId)
		throw std::runtime_error("WebRTC not supported");

	rtcSetUserPointer(mId, this);
	rtcSetDataChannelCallback(mId, DataChannelCallback);
	rtcSetLocalDescriptionCallback(mId, DescriptionCallback);
	rtcSetLocalCandidateCallback(mId, CandidateCallback);
	rtcSetStateChangeCallback(mId, StateChangeCallback);
	rtcSetGatheringStateChangeCallback(mId, GatheringStateChangeCallback);
	rtcSetSignalingStateChangeCallback(mId, SignalingStateChangeCallback);
}

PeerConnection::~PeerConnection() { rtcDeletePeerConnection(mId); }

optional<Description> PeerConnection::localDescription() const {
	// TODO: sdp and type of the local description in acquired by two function calls.
	// Find a way to reduce this into one call (probably multi-value return may work).
	char *sdp = rtcLocalDescriptionSdp(mId);
	char *type = rtcLocalDescriptionType(mId);
	if (!sdp || !type) {
		if (sdp)
			free(sdp);
		if (type)
			free(type);
		return std::nullopt;
	}
	// In the return value of rtcLocalDescription, type comes in front of sdp.
	Description description(sdp, type);
	free(sdp);
	free(type);
	return description;
}

shared_ptr<DataChannel> PeerConnection::createDataChannel(const string &label) {
	return std::make_shared<DataChannel>(rtcCreateDataChannel(mId, label.c_str()));
}

void PeerConnection::setRemoteDescription(const Description &description) {
	rtcSetRemoteDescription(mId, string(description).c_str(), description.typeString().c_str());
}

void PeerConnection::addRemoteCandidate(const Candidate &candidate) {
	rtcAddRemoteCandidate(mId, candidate.candidate().c_str(), candidate.mid().c_str());
}

void PeerConnection::onDataChannel(function<void(shared_ptr<DataChannel>)> callback) {
	mDataChannelCallback = callback;
}

void PeerConnection::onLocalDescription(function<void(const Description &)> callback) {
	mLocalDescriptionCallback = callback;
}

void PeerConnection::onLocalCandidate(function<void(const Candidate &)> callback) {
	mLocalCandidateCallback = callback;
}

void PeerConnection::onStateChange(std::function<void(State state)> callback) {
	mStateChangeCallback = callback;
}

void PeerConnection::onGatheringStateChange(std::function<void(GatheringState state)> callback) {
	mGatheringStateChangeCallback = callback;
}

void PeerConnection::onSignalingStateChange(std::function<void(SignalingState state)> callback) {
	mSignalingStateChangeCallback = callback;
}

void PeerConnection::triggerDataChannel(shared_ptr<DataChannel> dataChannel) {
	if (mDataChannelCallback)
		mDataChannelCallback(dataChannel);
}

void PeerConnection::triggerLocalDescription(const Description &description) {
	if (mLocalDescriptionCallback)
		mLocalDescriptionCallback(description);
}

void PeerConnection::triggerLocalCandidate(const Candidate &candidate) {
	if (mLocalCandidateCallback)
		mLocalCandidateCallback(candidate);
}

void PeerConnection::triggerStateChange(State state) {
	if (mStateChangeCallback)
		mStateChangeCallback(state);
}

void PeerConnection::triggerGatheringStateChange(GatheringState state) {
	if (mGatheringStateChangeCallback)
		mGatheringStateChangeCallback(state);
}

void PeerConnection::triggerSignalingStateChange(SignalingState state) {
	if (mSignalingStateChangeCallback)
		mSignalingStateChangeCallback(state);
}
} // namespace rtc

std::ostream &operator<<(std::ostream &out, const rtc::Candidate &candidate) {
	return out << std::string(candidate);
}

std::ostream &operator<<(std::ostream &out, const rtc::Description &description) {
	return out << std::string(description);
}
