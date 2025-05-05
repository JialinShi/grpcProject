#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include <grpcpp/grpcpp.h>
#include "chat.pb.h"
#include "chat.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

using chat::ChatMessage;
using chat::ChatService;

void ChatLoop(std::shared_ptr<ClientReaderWriter<ChatMessage,ChatMessage>> stream,
                const std::string& username){
     // 发消息线程               
    std::thread writer(
        //定义一个匿名函数，里面可以访问stream 和 username
        [stream, username](){
            while(true){
                std::string input;
                std::getline(std::cin,input);

                if(input == "/exit"){
                    stream->WritesDone();  // 告诉 server 我发完了
                    break;
                }

                ChatMessage msg;
                msg.set_username(username);
                msg.set_message(input);
                msg.set_timestamp(std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1));
                stream->Write(msg);

            }
        }
    );

    // 收消息主线程
    ChatMessage incoming;
    while(stream->Read(&incoming)){
        std::cout << "[" << incoming.username() << "]" 
        << incoming.message() << std::endl;
    }

    writer.join();
                
}

int main(){
    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin,username); //从终端读一整行（用户名可能包含空格）

    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    std::unique_ptr<ChatService::Stub> stub = ChatService::NewStub(channel);

    ClientContext context;
    std::shared_ptr<ClientReaderWriter<ChatMessage, ChatMessage>> stream = stub->Chat(&context);

    ChatLoop(stream, username);

    grpc::Status status = stream->Finish();
    if (!status.ok()) {
        std::cerr << "Chat RPC failed: " << status.error_message() << std::endl;
    }

    return 0;
}