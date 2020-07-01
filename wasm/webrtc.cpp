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

#include "webrtc.hpp"

#include <emscripten/emscripten.h>

#include <exception>
#include <iostream>

extern "C" {
extern int rtcCreatePeerConnection(const char **iceServers);
extern void rtcDeletePeerConnection(int pc);
extern int rtcCreateDataChannel(int pc, const char *label);
extern void rtcDeleteDataChannel(int dc);
extern void rtcSetDataChannelCallback(int pc, void (*dataChannelCallback)(int, void *));
extern void rtcSetLocalDescriptionCallback(int pc,
                                           void (*descriptionCallback)(const char *, const char *,
                                                                       void *));
extern void rtcSetLocalCandidateCallback(int pc, void (*candidateCallback)(const char *,
                                                                           const char *, void *));
extern void rtcSetRemoteDescription(int pc, const char *sdp, const char *type);
extern void rtcAddRemoteCandidate(int pc, const char *candidate, const char *mid);
extern int rtcGetDataChannelLabel(int dc, char *buffer, int size);
extern void rtcSetOpenCallback(int dc, void (*openCallback)(void *));
extern void rtcSetErrorCallback(int dc, void (*errorCallback)(const char *, void *));
extern void rtcSetMessageCallback(int dc, void (*messageCallback)(const char *, int, void *));
extern int rtcSendMessage(int dc, const char *buffer, int size);
extern void rtcSetUserPointer(int i, void *ptr);
}

namespace rtc {

using std::nullopt;

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...)->overloaded<Ts...>;

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

void DataChannel::OpenCallback(void *ptr) {
	DataChannel *d = static_cast<DataChannel *>(ptr);
	if (d) {
		d->mConnected = true;
		d->triggerOpen();
	}
}

void DataChannel::ErrorCallback(const char *error, void *ptr) {
	DataChannel *d = static_cast<DataChannel *>(ptr);
	if (d)
		d->triggerError(string(error ? error : "unknown"));
}

void DataChannel::MessageCallback(const char *data, int size, void *ptr) {
	DataChannel *d = static_cast<DataChannel *>(ptr);
	if (d) {
		if (data) {
			if (size >= 0) {
				auto *b = reinterpret_cast<const byte *>(data);
				d->triggerMessage(binary(b, b + size));
			} else {
				d->triggerMessage(string(data));
			}
		} else {
			d->close();
			d->triggerClosed();
		}
	}
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
}

PeerConnection::~PeerConnection(void) { rtcDeletePeerConnection(mId); }

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

DataChannel::DataChannel(int id) : mId(id), mConnected(false) {
	rtcSetUserPointer(mId, this);
	rtcSetOpenCallback(mId, OpenCallback);
	rtcSetErrorCallback(mId, ErrorCallback);
	rtcSetMessageCallback(mId, MessageCallback);

	char str[256];
	rtcGetDataChannelLabel(mId, str, 256);
	mLabel = str;
}

DataChannel::~DataChannel(void) { close(); }

void DataChannel::close(void) {
	mConnected = false;
	if (mId) {
		rtcDeleteDataChannel(mId);
		mId = 0;
	}
}

void DataChannel::send(const std::variant<binary, string> &message) {
	if (!mId)
		return;
	std::visit(overloaded{[this](const binary &b) {
		                      auto data = reinterpret_cast<const char *>(b.data());
		                      rtcSendMessage(mId, data, b.size());
	                      },
	                      [this](const string &s) { rtcSendMessage(mId, s.c_str(), -1); }},
	           message);
}

bool DataChannel::isOpen(void) const {
		return mConnected; }

bool DataChannel::isClosed(void) const {
		return mId == 0; }

std::string DataChannel::label(void) const {
		return mLabel; }

} // namespace rtc

std::ostream &operator<<(std::ostream &out, const rtc::Candidate &candidate) {
	return out << std::string(candidate);
}

std::ostream &operator<<(std::ostream &out, const rtc::Description &description) {
	return out << std::string(description);
}
