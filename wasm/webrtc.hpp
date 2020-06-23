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

#ifndef RTC_WEBRTC_H
#define RTC_WEBRTC_H

#include "channel.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

namespace rtc {

using std::shared_ptr;
using std::vector;

struct Configuration {
	vector<string> iceServers;
};

class Description {
public:
	Description(const string &sdp, const string &type) : mSdp(sdp), mType(type) {}
	string typeString() const { return mType; }
	operator string() const { return mSdp; }

private:
	string mSdp;
	string mType;
};

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

class DataChannel : public Channel {
public:
	explicit DataChannel(int id);
	~DataChannel();

	void close();
	void send(const std::variant<binary, string> &data);

	bool isOpen() const;
	bool isClosed() const;

	string label() const;

private:
	int mId;
	string mLabel;
	bool mConnected;

	static void OpenCallback(void *ptr);
	static void ErrorCallback(const char *error, void *ptr);
	static void MessageCallback(const char *data, int size, void *ptr);
};

class PeerConnection {
public:
	PeerConnection();
	PeerConnection(const Configuration &config);
	~PeerConnection();

	shared_ptr<DataChannel> createDataChannel(const string &label);

	void setRemoteDescription(const Description &description);
	void addRemoteCandidate(const Candidate &candidate);

	void onDataChannel(std::function<void(shared_ptr<DataChannel>)> callback);
	void onLocalDescription(std::function<void(const Description &description)> callback);
	void onLocalCandidate(std::function<void(const Candidate &candidate)> callback);

protected:
	void triggerDataChannel(shared_ptr<DataChannel> dataChannel);
	void triggerLocalDescription(const Description &description);
	void triggerLocalCandidate(const Candidate &candidate);

	std::function<void(shared_ptr<DataChannel>)> mDataChannelCallback;
	std::function<void(const Description &description)> mLocalDescriptionCallback;
	std::function<void(const Candidate &candidate)> mLocalCandidateCallback;

private:
	int mId;

	static void DataChannelCallback(int dc, void *ptr);
	static void DescriptionCallback(const char *sdp, const char *type, void *ptr);
	static void CandidateCallback(const char *candidate, const char *mid, void *ptr);
};

} // namespace rtc

std::ostream &operator<<(std::ostream &out, const Candidate &candidate);
std::ostream &operator<<(std::ostream &out, const Description &description);

#endif // RTC_WEBRTC_H
