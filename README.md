# datachannel-wasm - C++ Wasm and Native WebRTC Data Channels

datachannel-wasm is a C++ WebRTC Data Channels and WebSocket wrapper for [Emscripten](https://emscripten.org/). It can be used transparently with CMake for both WebAssembly targets and native targets. For native targets, it will switch to linking statically against [libdatachannel](https://github.com/paullouisageneau/libdatachannel).

The interface is a subset of the one of [libdatachannel](https://github.com/paullouisageneau/libdatachannel), see what is available in [wasm/include](https://github.com/paullouisageneau/datachannel-wasm/tree/master/wasm/include/rtc).

These wrappers were originally written for my multiplayer game [Convergence](https://github.com/paullouisageneau/convergence) and were extracted from there to be easily reusable.

## Installation

### Add the submodule to your project
```bash
$ git submodule add https://github.com/paullouisageneau/datachannel-wasm.git deps/datachannel-wasm
$ git submodule update --init --recursive
```

### Register the library in CMakeLists.txt
```cmake
[...]
add_subdirectory(deps/datachannel-wasm EXCLUDE_FROM_ALL)
target_link_libraries(my_project datachannel-wasm)
```

## Building

### Wasm with Emscripten

Building with emscripten will link against the JavaScript WebRTC and WebSocket wrapper.

This requires that you have [emsdk](https://github.com/emscripten-core/emsdk) installed and activated in your environment.

```bash
$ cmake -B build-emscripten -DCMAKE_TOOLCHAIN_FILE=$EMSDK/emscripten/master/cmake/Modules/Platform/Emscripten.cmake
$ cd build-emscripten
$ make -j2
```

### Native

Building for a non-emscripten target will link statically against [libdatachannel](https://github.com/paullouisageneau/libdatachannel).

```bash
$ cmake -B build-native
$ cd build-native
$ make -j2
```

Non-emscripten cross-compilation will work as expected and link statically against [libdatachannel](https://github.com/paullouisageneau/libdatachannel), for instance compiling for Microsoft Windows with MinGW cross-compilation.

```bash
$ cmake -B build -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-x86_64-w64-mingw32.cmake # replace with your toolchain file
$ cd build
$ make -j2
```

Note compilation will also work on Microsoft Windows with Microsoft Visual C++.

```bash
$ cmake -B build -G "NMake Makefiles"
$ cd build
$ nmake
```

