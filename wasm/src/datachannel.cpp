/*************************************************************************
 *   Copyright (C) 2017-2020 by Paul-Louis Ageneau                       *
 *   paul-louis (at) ageneau (dot) org                                   *
 *                                                                       *
 *   This file is part of datachannel-wasm.                              *
 *                                                                       *
 *   datachannel-wasm is free software: you can redistribute it and/or   *
 *   modify it under the terms of the GNU Affero General Public License  *
 *   as published by the Free Software Foundation, either version 3 of   *
 *   the License, or (at your option) any later version.                 *
 *                                                                       *
 *   datachannel-wasm is distributed in the hope that it will be useful, *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the        *
 *   GNU Affero General Public License for more details.                 *
 *                                                                       *
 *   You should have received a copy of the GNU Affero General Public    *
 *   License along with datachannel-wasm.                                *
 *   If not, see <http://www.gnu.org/licenses/>.                         *
 *************************************************************************/

#include "datachannel.hpp"

#include <emscripten/emscripten.h>

#include <exception>
#include <stdexcept>

extern "C" {
extern void rtcDeleteDataChannel(int dc);
extern int rtcGetDataChannelLabel(int dc, char *buffer, int size);
extern void rtcSetOpenCallback(int dc, void (*openCallback)(void *));
extern void rtcSetErrorCallback(int dc, void (*errorCallback)(const char *, void *));
extern void rtcSetMessageCallback(int dc, void (*messageCallback)(const char *, int, void *));
extern void rtcSetBufferedAmountLowCallback(int dc, void (*bufferedAmountLowCallback)(void *));
extern int rtcGetBufferedAmount(int dc);
extern void rtcSetBufferedAmountLowThreshold(int dc, int threshold);
extern int rtcSendMessage(int dc, const char *buffer, int size);
extern void rtcSetUserPointer(int i, void *ptr);
}

namespace rtc {

using std::function;
using std::shared_ptr;

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

void DataChannel::BufferedAmountLowCallback(void *ptr) {
	DataChannel *d = static_cast<DataChannel *>(ptr);
	if (d) {
		d->triggerBufferedAmountLow();
	}
}

DataChannel::DataChannel(int id) : mId(id), mConnected(false) {
	rtcSetUserPointer(mId, this);
	rtcSetOpenCallback(mId, OpenCallback);
	rtcSetErrorCallback(mId, ErrorCallback);
	rtcSetMessageCallback(mId, MessageCallback);
	rtcSetBufferedAmountLowCallback(mId, BufferedAmountLowCallback);

	char str[256];
	rtcGetDataChannelLabel(mId, str, 256);
	mLabel = str;
}

DataChannel::~DataChannel() { close(); }

void DataChannel::close() {
	mConnected = false;
	if (mId) {
		rtcDeleteDataChannel(mId);
		mId = 0;
	}
}

bool DataChannel::send(message_variant message) {
	if (!mId)
		return false;

	return std::visit(
	    overloaded{[this](const binary &b) {
		               auto data = reinterpret_cast<const char *>(b.data());
		               return rtcSendMessage(mId, data, int(b.size())) >= 0;
	               },
	               [this](const string &s) { return rtcSendMessage(mId, s.c_str(), -1) >= 0; }},
	    std::move(message));
}

bool DataChannel::send(const byte *data, size_t size) {
	if (!mId)
		return false;

	return rtcSendMessage(mId, reinterpret_cast<const char *>(data), int(size)) >= 0;
}

bool DataChannel::isOpen() const { return mConnected; }

bool DataChannel::isClosed() const { return mId == 0; }

size_t DataChannel::bufferedAmount() const {
	if(!mId)
		return 0;

	int ret = rtcGetBufferedAmount(mId);
	if(ret < 0) return 0;

	return size_t(ret);
}

std::string DataChannel::label() const { return mLabel; }

void DataChannel::setBufferedAmountLowThreshold(size_t amount) {
	if(!mId) return;

	rtcSetBufferedAmountLowThreshold(mId, int(amount));
}

} // namespace rtc
