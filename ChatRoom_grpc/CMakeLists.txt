cmake_minimum_required(VERSION 3.15)
project(ChatRoom_grpc)

set(CMAKE_CXX_STANDARD 17)

# 找到 Protobuf 和 gRPC
find_package(Protobuf CONFIG REQUIRED PATHS "/Users/yuhao-m3max/.local/lib/cmake/protobuf")
find_package(gRPC CONFIG REQUIRED PATHS "/Users/yuhao-m3max/.local/lib/cmake/grpc")




# 包含 proto 文件头目录 + 生成文件目录
include_directories(
  ${Protobuf_INCLUDE_DIRS}
  ${gRPC_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/proto
)

# Proto 文件生成的源代码路径
set(PROTO_SRC
  proto/chat.pb.cc
  proto/chat.grpc.pb.cc
)

# Chat server
add_executable(chat_server
  server/chat_server.cc
  ${PROTO_SRC}
)
target_link_libraries(chat_server
  gRPC::grpc++
  protobuf::libprotobuf
)

# Chat client
add_executable(chat_client
  client/chat_client.cc
  ${PROTO_SRC}
)
target_link_libraries(chat_client
  gRPC::grpc++
  protobuf::libprotobuf
)
