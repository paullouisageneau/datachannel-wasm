cmake_minimum_required(VERSION 3.7)
project(datachannel-wasm
	DESCRIPTION "C++ WebRTC Data Channels for WebAssembly in browsers"
	VERSION 0.3.2
	LANGUAGES CXX)

if(NOT CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	message(FATAL_ERROR "datachannel-wasm must be compiled with Emscripten.")
endif()

set(WASM_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/wasm/src)
set(DATACHANNELS_SRC
	${WASM_SRC_DIR}/candidate.cpp
	${WASM_SRC_DIR}/channel.cpp
	${WASM_SRC_DIR}/configuration.cpp
	${WASM_SRC_DIR}/description.cpp
	${WASM_SRC_DIR}/datachannel.cpp
	${WASM_SRC_DIR}/global.cpp
	${WASM_SRC_DIR}/peerconnection.cpp
	${WASM_SRC_DIR}/websocket.cpp)

add_library(datachannel-wasm STATIC ${DATACHANNELS_SRC})
set_target_properties(datachannel-wasm PROPERTIES
	VERSION ${PROJECT_VERSION}
	CXX_STANDARD 17)

target_include_directories(datachannel-wasm PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/wasm/include)
target_include_directories(datachannel-wasm PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/wasm/include/rtc)

target_link_options(datachannel-wasm PUBLIC
	"SHELL:--js-library \"${CMAKE_CURRENT_SOURCE_DIR}/wasm/js/webrtc.js\""
	"SHELL:--js-library \"${CMAKE_CURRENT_SOURCE_DIR}/wasm/js/websocket.js\"")

