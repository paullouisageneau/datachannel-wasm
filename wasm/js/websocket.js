/**
 * Copyright (C) 2017-2020 by Paul-Louis Ageneau
 * paul-louis (at) ageneau (dot) org
 *
 * This file is part of datachannel-wasm.
 *
 * datachannel-wasm is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * datachannel-wasm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with datachannel-wasm.
 * If not, see <http://www.gnu.org/licenses/>.
 */

(function() {
	var WebSocket = {
		$WEBSOCKET: {
			map: {},
			nextId: 1,

			allocUTF8FromString: function(str) {
				var strLen = lengthBytesUTF8(str);
				var strOnHeap = _malloc(strLen+1);
				stringToUTF8(str, strOnHeap, strLen+1);
				return strOnHeap;
			},

			registerWebSocket: function(webSocket) {
				var ws = WEBSOCKET.nextId++;
				WEBSOCKET.map[ws] = webSocket;
				webSocket.binaryType = 'arraybuffer';
				return ws;
			},
		},

		wsCreateWebSocket: function(pUrl) {
			var url = UTF8ToString(pUrl);
			if(!window.WebSocket) return 0;
			return WEBSOCKET.registerWebSocket(new WebSocket(url));
		},

		wsDeleteWebSocket: function(ws) {
			var webSocket = WEBSOCKET.map[ws];
			if(webSocket) {
				webSocket.close();
				webSocket.wsUserDeleted = true;
				delete WEBSOCKET.map[ws];
			}
		},

		wsSetOpenCallback: function(ws, openCallback) {
			var webSocket = WEBSOCKET.map[ws];
			var cb = function() {
				if(webSocket.rtcUserDeleted) return;
				var userPointer = webSocket.rtcUserPointer || 0;
				Module['dynCall_vi'](openCallback, userPointer);
			};
			webSocket.onopen = cb;
			if(webSocket.readyState == 1) setTimeout(cb, 0);
		},

 		wsSetErrorCallback: function(ws, errorCallback) {
			var webSocket = WEBSOCKET.map[ws];
			var cb = function() {
				if(webSocket.rtcUserDeleted) return;
				var userPointer = webSocket.rtcUserPointer || 0;
				Module['dynCall_vii'](errorCallback, 0, userPointer);
			};
			webSocket.onerror = cb;
		},

		wsSetMessageCallback: function(ws, messageCallback) {
			var webSocket = WEBSOCKET.map[ws];
			webSocket.onmessage = function(evt) {
				if(webSocket.rtcUserDeleted) return;
				if(typeof evt.data == 'string') {
					var pStr = WEBRTC.allocUTF8FromString(evt.data);
					var userPointer = webSocket.rtcUserPointer || 0;
					Module['dynCall_viii'](messageCallback, pStr, -1, userPointer);
					_free(pStr);
				} else {
					var byteArray = new Uint8Array(evt.data);
					var size = byteArray.byteLength;
					var pBuffer = _malloc(size);
					var heapBytes = new Uint8Array(Module['HEAPU8'].buffer, pBuffer, size);
					heapBytes.set(byteArray);
					var userPointer = webSocket.rtcUserPointer || 0;
					Module['dynCall_viii'](messageCallback, pBuffer, size, userPointer);
					_free(pBuffer);
				}
			};
			webSocket.onclose = function() {
				if(webSocket.rtcUserDeleted) return;
				var userPointer = webSocket.rtcUserPointer || 0;
				Module['dynCall_viii'](messageCallback, 0, 0, userPointer);
			};
		},

		wsSendMessage: function(ws, pBuffer, size) {
			var webSocket = WEBSOCKET.map[ws];
			if(webSocket.readyState != 1) return 0;
			if(size >= 0) {
				var heapBytes = new Uint8Array(Module['HEAPU8'].buffer, pBuffer, size);
				webSocket.send(heapBytes);
				return size;
			} else {
				var str = UTF8ToString(pBuffer);
				webSocket.send(str);
				return lengthBytesUTF8(str);
			}
		},

		wsSetUserPointer: function(ws, ptr) {
			var webSocket = WEBSOCKET.map[ws];
			if(webSocket) webSocket.rtcUserPointer = ptr;
		},
	};

	autoAddDeps(WebSocket, '$WEBSOCKET');
	mergeInto(LibraryManager.library, WebSocket);
})();

