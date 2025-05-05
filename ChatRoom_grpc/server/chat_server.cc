#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include <grpcpp/grpcpp.h>
#include "chat.pb.h"
#include "chat.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

using chat::ChatMessage;
using chat::ChatService;


class ChatServiceImpl final: public ChatService::Service{
public:
    Status Chat(ServerContext* context,
                ServerReaderWriter<ChatMessage, ChatMessage>* stream) override{
        {
            std::lock_guard<std::mutex> lock(mu_);
            clients_.push_back(stream);
        }

        ChatMessage msg;
        while(stream->Read(&msg)){
            std::lock_guard<std::mutex> lock(mu_);
            for(auto client:clients){
                client->Write(msg);
            }
        }

        {
            std::lock guard<std::mutex> lock(mu_);
            clients_erase(std::remove(clients_.begin(). clients_end(), stream),clients_.end());
        }

        return Status::OK;
    }
private:
    std::vector<ServerReaderWriter<ChatMessage, ChatMessage>*> clients_;
    std::mutex mu_;
};

void RunServer(){
    std::string server_address("0.0.0.0:50051");
    ChatServiceImpl service;
    ServerBuilder builder;
      
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Chat server listening on " << server_address << std::endl;
    server->Wait();
}

int main(){
    RunServer();
    return 0;
}