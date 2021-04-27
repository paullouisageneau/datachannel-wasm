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

(function() {
	var WebRTC = {
		$WEBRTC: {
			peerConnectionsMap: {},
			dataChannelsMap: {},
			nextId: 1,

			allocUTF8FromString: function(str) {
				var strLen = lengthBytesUTF8(str);
				var strOnHeap = _malloc(strLen+1);
				stringToUTF8(str, strOnHeap, strLen+1);
				return strOnHeap;
			},

			registerPeerConnection: function(peerConnection) {
				var pc = WEBRTC.nextId++;
				WEBRTC.peerConnectionsMap[pc] = peerConnection;
				peerConnection.onnegotiationneeded = function() {
					peerConnection.createOffer()
						.then(function(offer) {
							return WEBRTC.handleDescription(peerConnection, offer);
						})
						.catch(function(err) {
							console.error(err);
						});
				};
				peerConnection.onicecandidate = function(evt) {
					if(evt.candidate && evt.candidate.candidate)
					  WEBRTC.handleCandidate(peerConnection, evt.candidate);
				};
				peerConnection.onconnectionstatechange = function(evt) {
					WEBRTC.handleConnectionStateChange(peerConnection, peerConnection.connectionState)
				};
				peerConnection.onicegatheringstatechange = function() {
					WEBRTC.handleIceGatheringStateChange(peerConnection, peerConnection.iceGatheringState)
				};
				peerConnection.onsignalingstatechange = function(evt) {
					WEBRTC.handleSignalingStateChange(peerConnection, peerConnection.signalingState)
				};
				return pc;
			},

			registerDataChannel: function(dataChannel) {
				var dc = WEBRTC.nextId++;
				WEBRTC.dataChannelsMap[dc] = dataChannel;
				dataChannel.binaryType = 'arraybuffer';
				return dc;
			},

			handleDescription: function(peerConnection, description) {
				return peerConnection.setLocalDescription(description)
					.then(function() {
						if(peerConnection.rtcUserDeleted) return;
						if(!peerConnection.rtcDescriptionCallback) return;
						var desc = peerConnection.localDescription;
						var pSdp = WEBRTC.allocUTF8FromString(desc.sdp);
						var pType = WEBRTC.allocUTF8FromString(desc.type);
						var callback =  peerConnection.rtcDescriptionCallback;
						var userPointer = peerConnection.rtcUserPointer || 0;
						Module['dynCall']('viii', callback, [pSdp, pType, userPointer]);
						_free(pSdp);
						_free(pType);
					});
			},

			handleCandidate: function(peerConnection, candidate) {
				if(peerConnection.rtcUserDeleted) return;
				if(!peerConnection.rtcCandidateCallback) return;
				var pCandidate = WEBRTC.allocUTF8FromString(candidate.candidate);
				var pSdpMid = WEBRTC.allocUTF8FromString(candidate.sdpMid);
				var candidateCallback =  peerConnection.rtcCandidateCallback;
				var userPointer = peerConnection.rtcUserPointer || 0;
				Module['dynCall']('viii', candidateCallback, [pCandidate, pSdpMid, userPointer]);
				_free(pCandidate);
				_free(pSdpMid);
			},

			handleConnectionStateChange: function(peerConnection, connectionState) {
				var stateChangeCallback =  peerConnection.rtcStateChangeCallback;
				var userPointer = peerConnection.rtcUserPointer || 0;
				switch(connectionState) {
					case 'new':
						Module['dynCall']('vii', stateChangeCallback, [0, userPointer]);
						break;
					case 'connecting':
						Module['dynCall']('vii', stateChangeCallback, [1, userPointer]);
						break;
					case 'connected':
						Module['dynCall']('vii', stateChangeCallback, [2, userPointer]);
						break;
					case 'disconnected':
						Module['dynCall']('vii', stateChangeCallback, [3, userPointer]);
						break;
					case 'failed':
						Module['dynCall']('vii', stateChangeCallback, [4, userPointer]);
						break;
					case 'closed':
						Module['dynCall']('vii', stateChangeCallback, [5, userPointer]);
						break;
				}
			},

			handleIceGatheringStateChange: function(peerConnection, iceGatheringState) {
				var gatheringStateChangeCallback = peerConnection.rtcGatheringStateChangeCallback;
				var userPointer = peerConnection.rtcUserPointer || 0;
				switch(iceGatheringState) {
					case "new":
						Module['dynCall']('vii', gatheringStateChangeCallback, [0, userPointer]);
						break;
					case "gathering":
						Module['dynCall']('vii', gatheringStateChangeCallback, [1, userPointer]);
						break;
					case "complete":
						Module['dynCall']('vii', gatheringStateChangeCallback, [2, userPointer]);
						break;
				}
			},

			handleSignalingStateChange: function(peerConnection, signalingState) {
				var signalingStateChangeCallback = peerConnection.rtcSignalingStateChangeCallback;
				var userPointer = peerConnection.rtcUserPointer || 0;
				switch(signalingState) {
					case "stable":
						Module['dynCall']('vii', signalingStateChangeCallback, [0, userPointer]);
						break;
					case "have-local-offer":
						Module['dynCall']('vii', signalingStateChangeCallback, [1, userPointer]);
						break;
					case "have-remote-offer":
						Module['dynCall']('vii', signalingStateChangeCallback, [2, userPointer]);
						break;
					case "have-local-pranswer":
						Module['dynCall']('vii', signalingStateChangeCallback, [3, userPointer]);
						break;
					case "have-remote-pranswer":
						Module['dynCall']('vii', signalingStateChangeCallback, [4, userPointer]);
						break;
				}
			},
		},

		rtcCreatePeerConnection: function(pIceServers, nIceServers) {
			if(!window.RTCPeerConnection) return 0;
			var iceServers = [];
			for(var i = 0; i < nIceServers; ++i) {
			  var heap = Module['HEAPU32'];
				var p = heap[pIceServers/heap.BYTES_PER_ELEMENT + i];
				iceServers.push({
					urls: [UTF8ToString(p)],
				});
			}
			var config = {
				iceServers: iceServers,
			};
			return WEBRTC.registerPeerConnection(new RTCPeerConnection(config));
		},

		rtcDeletePeerConnection: function(pc) {
			var peerConnection = WEBRTC.peerConnectionsMap[pc];
			if(peerConnection) {
				peerConnection.rtcUserDeleted = true;
				delete WEBRTC.peerConnectionsMap[pc];
			}
		},

		rtcCreateDataChannel: function(pc, pLabel) {
			if(!pc) return 0;
			var label = UTF8ToString(pLabel);
			var peerConnection = WEBRTC.peerConnectionsMap[pc];
			var channel = peerConnection.createDataChannel(label);
			return WEBRTC.registerDataChannel(channel);
		},

 		rtcDeleteDataChannel: function(dc) {
			var dataChannel = WEBRTC.dataChannelsMap[dc];
			if(dataChannel) {
				dataChannel.rtcUserDeleted = true;
				delete WEBRTC.dataChannelsMap[dc];
			}
		},

		rtcSetDataChannelCallback: function(pc, dataChannelCallback) {
			if(!pc) return;
			var peerConnection = WEBRTC.peerConnectionsMap[pc];
			peerConnection.ondatachannel = function(evt) {
				if(peerConnection.rtcUserDeleted) return;
				var dc = WEBRTC.registerDataChannel(evt.channel);
				var userPointer = peerConnection.rtcUserPointer || 0;
				Module['dynCall']('vii', dataChannelCallback, [dc, userPointer]);
			};
		},

		rtcSetLocalDescriptionCallback: function(pc, descriptionCallback) {
			if(!pc) return;
			var peerConnection = WEBRTC.peerConnectionsMap[pc];
			peerConnection.rtcDescriptionCallback = descriptionCallback;
		},

		rtcSetLocalCandidateCallback: function(pc, candidateCallback) {
			if(!pc) return;
			var peerConnection = WEBRTC.peerConnectionsMap[pc];
			peerConnection.rtcCandidateCallback = candidateCallback;
		},

		rtcSetStateChangeCallback: function(pc, stateChangeCallback) {
			if(!pc) return;
			var peerConnection = WEBRTC.peerConnectionsMap[pc];
			peerConnection.rtcStateChangeCallback = stateChangeCallback;
		},

		rtcSetGatheringStateChangeCallback: function(pc, gatheringStateChangeCallback) {
			if(!pc) return;
			var peerConnection = WEBRTC.peerConnectionsMap[pc];
			peerConnection.rtcGatheringStateChangeCallback = gatheringStateChangeCallback;
		},

		rtcSetSignalingStateChangeCallback: function(pc, signalingStateChangeCallback) {
			if(!pc) return;
			var peerConnection = WEBRTC.peerConnectionsMap[pc];
			peerConnection.rtcSignalingStateChangeCallback = signalingStateChangeCallback;
		},

		rtcSetRemoteDescription: function(pc, pSdp, pType) {
			var description = new RTCSessionDescription({
				sdp: UTF8ToString(pSdp),
				type: UTF8ToString(pType),
			});
			var peerConnection = WEBRTC.peerConnectionsMap[pc];
			peerConnection.setRemoteDescription(description)
				.then(function() {
					if(peerConnection.rtcUserDeleted) return;
					if(description.type == 'offer') {
						peerConnection.createAnswer()
							.then(function(answer) {
								return WEBRTC.handleDescription(peerConnection, answer);
							})
							.catch(function(err) {
								console.error(err);
							});
					}
				})
				.catch(function(err) {
					console.error(err);
				});
		},

		rtcAddRemoteCandidate: function(pc, pCandidate, pSdpMid) {
			var iceCandidate = new RTCIceCandidate({
				candidate: UTF8ToString(pCandidate),
				sdpMid: UTF8ToString(pSdpMid),
			});
			var peerConnection = WEBRTC.peerConnectionsMap[pc];
			peerConnection.addIceCandidate(iceCandidate)
				.catch(function(err) {
					console.error(err);
				});
		},

		rtcGetDataChannelLabel: function(dc, pBuffer, size) {
			var label = WEBRTC.dataChannelsMap[dc].label;
			stringToUTF8(label, pBuffer, size);
			return lengthBytesUTF8(label);
		},

		rtcSetOpenCallback: function(dc, openCallback) {
			var dataChannel = WEBRTC.dataChannelsMap[dc];
			var cb = function() {
				if(dataChannel.rtcUserDeleted) return;
				var userPointer = dataChannel.rtcUserPointer || 0;
				Module['dynCall']('vi', openCallback, [userPointer]);
			};
			dataChannel.onopen = cb;
			if(dataChannel.readyState == 'open') setTimeout(cb, 0);
		},

		rtcSetErrorCallback: function(dc, errorCallback) {
			var dataChannel = WEBRTC.dataChannelsMap[dc];
			var cb = function(evt) {
				if(dataChannel.rtcUserDeleted) return;
				var userPointer = dataChannel.rtcUserPointer || 0;
				var pError = evt.message ? WEBRTC.allocUTF8FromString(evt.message) : 0;
				Module['dynCall']('vii', errorCallback, [pError, userPointer]);
				_free(pError);
			};
			dataChannel.onerror = cb;
		},

		rtcSetMessageCallback: function(dc, messageCallback) {
			var dataChannel = WEBRTC.dataChannelsMap[dc];
			dataChannel.onmessage = function(evt) {
				if(dataChannel.rtcUserDeleted) return;
				var userPointer = dataChannel.rtcUserPointer || 0;
				if(typeof evt.data == 'string') {
					var pStr = WEBRTC.allocUTF8FromString(evt.data);
					Module['dynCall']('viii', messageCallback, [pStr, -1, userPointer]);
					_free(pStr);
				} else {
					var byteArray = new Uint8Array(evt.data);
					var size = byteArray.length;
					var pBuffer = _malloc(size);
					var heapBytes = new Uint8Array(Module['HEAPU8'].buffer, pBuffer, size);
					heapBytes.set(byteArray);
					Module['dynCall']('viii', messageCallback, [pBuffer, size, userPointer]);
					_free(pBuffer);
				}
			};
			dataChannel.onclose = function() {
				if(dataChannel.rtcUserDeleted) return;
				var userPointer = dataChannel.rtcUserPointer || 0;
				Module['dynCall']('viii', messageCallback, [0, 0, userPointer]);
			};
		},

		rtcSetBufferedAmountLowCallback: function(dc, bufferedAmountLowCallback) {
			var dataChannel = WEBRTC.dataChannelsMap[dc];
			var cb = function(evt) {
				if(dataChannel.rtcUserDeleted) return;
				var userPointer = dataChannel.rtcUserPointer || 0;
					Module['dynCall']('vi', bufferedAmountLowCallback, [userPointer]);
			};
			dataChannel.onbufferedamountlow = cb;
		},

		rtcGetBufferedAmount: function(dc) {
			var dataChannel = WEBRTC.dataChannelsMap[dc];
			return dataChannel.bufferedAmount;
		},

		rtcSetBufferedAmountLowThreshold: function(dc, threshold) {
			var dataChannel = WEBRTC.dataChannelsMap[dc];
			dataChannel.bufferedAmountLowThreshold = threshold;
		},

		rtcSendMessage: function(dc, pBuffer, size) {
			var dataChannel = WEBRTC.dataChannelsMap[dc];
			if(dataChannel.readyState != 'open') return 0;
			if(size >= 0) {
				var heapBytes = new Uint8Array(Module['HEAPU8'].buffer, pBuffer, size);
				dataChannel.send(heapBytes);
				return size;
			} else {
				var str = UTF8ToString(pBuffer);
				dataChannel.send(str);
				return lengthBytesUTF8(str);
			}
		},

		rtcSetUserPointer: function(i, ptr) {
			if(WEBRTC.peerConnectionsMap[i]) WEBRTC.peerConnectionsMap[i].rtcUserPointer = ptr;
			if(WEBRTC.dataChannelsMap[i]) WEBRTC.dataChannelsMap[i].rtcUserPointer = ptr;
		},
	};

	autoAddDeps(WebRTC, '$WEBRTC');
	mergeInto(LibraryManager.library, WebRTC);
})();
