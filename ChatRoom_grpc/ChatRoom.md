# 1. Environment Setup

OS: macOS (Apple Silicon M3)

Compiler: AppleClang 15.0.0

CMake Version: 3.31

Protobuf: Installed from source

gRPC: Installed from source

Install Path: $HOME/.local

Export Environment Variable:

```export grpcProject=$HOME/.local```

# 2. Protobuf & gRPC Installation

Protobuf:

Initially installed Protobuf v25.1.0

Encountered issues due to missing dependencies (absl, utf8_range)

Resolution: Replaced with stable Protobuf v21.12 to avoid these external dependencies

gRPC:

Installed with:
```
cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_CXX_STANDARD=17 \
      -DCMAKE_INSTALL_PREFIX=$grpcProject \
      ../..
make -j4
make install
```

# 3. ChatRoom Project Setup

File Structure:
```
ChatRoom_grpc/
  |- client/
  |- server/
  |- proto/
  |- CMakeLists.txt
```
Common Issue:

```CMake Error: ProtobufConfig.cmake found but Protobuf_FOUND=FALSE```

This happened due to ```find_package(Protobuf CONFIG REQUIRED)``` being strict about absl dependencies

Solution: Use stable version of protobuf (v21.12) or avoid CONFIG mode altogether

# 4. CMakeLists.txt (Final Working Version)
```
cmake_minimum_required(VERSION 3.15)
project(ChatRoom_grpc)

set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf CONFIG REQUIRED PATHS "/Users/yuhao-m3max/.local/lib/cmake/protobuf")
find_package(gRPC CONFIG REQUIRED PATHS "/Users/yuhao-m3max/.local/lib/cmake/grpc")

include_directories(
  ${Protobuf_INCLUDE_DIRS}
  ${gRPC_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/proto
)

set(PROTO_SRC
  proto/chat.pb.cc
  proto/chat.grpc.pb.cc
)

add_executable(chat_server server/chat_server.cc ${PROTO_SRC})
target_link_libraries(chat_server gRPC::grpc++ protobuf::libprotobuf)

add_executable(chat_client client/chat_client.cc ${PROTO_SRC})
target_link_libraries(chat_client gRPC::grpc++ protobuf::libprotobuf)
```
# 5. Build Process

From the project root:

```rm -rf cmake/build
mkdir -p cmake/build
cd cmake/build
cmake ../..
make -j4
```
## Checkpoints:

Ensure both gRPC and Protobuf are found in cmake logs

-- Found Protobuf: /Users/.../.local/lib/libprotobuf.a
-- Found gRPC: /Users/.../.local/lib/libgrpc++.a

# 6. Lessons Learned

Use stable Protobuf versions to avoid dependency issues

Avoid relying on CONFIG mode unless you're sure about dependencies

Always clear and reconfigure build when changing CMake variables or paths

Relative paths are critical in CMake (cmake ../.. must point to correct root)

Use find_package(... CONFIG REQUIRED PATHS ...) for deterministic behavior

7. Next Step

Proceed to clean reimplementation of ChatRoom with:

Fresh proto file

Minimal server/client

Manually controlled MakeLists.txt