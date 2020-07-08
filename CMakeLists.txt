cmake_minimum_required(VERSION 3.7)
project(datachannels
	DESCRIPTION "C++ Wasm and Native Data Channels"
	VERSION 0.1.0
	LANGUAGES CXX)

set(DATACHANNELS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/datachannels.cpp)

if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    set(DATACHANNELS_SOURCES ${DATACHANNEL_SOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/wasm/channel.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/wasm/webrtc.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/wasm/websocket.cpp)
endif()

add_library(datachannels STATIC ${DATACHANNELS_SOURCES})
set_target_properties(datachannels PROPERTIES
	VERSION ${PROJECT_VERSION}
	CXX_STANDARD 17)

if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
	target_include_directories(datachannels PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/wasm)

    em_link_js_library(datachannels
		${CMAKE_CURRENT_SOURCE_DIR}/wasm/js/webrtc.js
		${CMAKE_CURRENT_SOURCE_DIR}/wasm/js/websocket.js
		${CMAKE_CURRENT_SOURCE_DIR}/wasm/js/pre.js)

    set(CMAKE_EXECUTABLE_SUFFIX ".html")

	set(WASM_OPTS
		-Oz -g0
		"SHELL:-s WASM=1"
		"SHELL:-s BINARYEN_METHOD=native-wasm"
		"SHELL:-s TOTAL_MEMORY=256MB"
		"SHELL:-s DISABLE_EXCEPTION_CATCHING=0")

    target_compile_options(datachannels PUBLIC ${WASM_OPTS})
    target_link_options(datachannels PUBLIC ${WASM_OPTS} --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/html/index.html)

else()
	target_include_directories(datachannels PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/native)

	option(USE_JUICE "Use libjuice instead of libnice in libdatachannel" ON)
	option(NO_WEBSOCKET "Disable WebSocket support in libdatachannel" OFF)
	option(NO_EXAMPLES "Disable examples in libdatachannel" ON)

	add_subdirectory(deps/libdatachannel EXCLUDE_FROM_ALL)
	target_link_libraries(datachannels PUBLIC datachannel-static)
endif()

