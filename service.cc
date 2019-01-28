#include "service.h"

#include <grpcpp/grpcpp.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using chirp::Chirp;
using chirp::RegisterRequest;
using chirp::RegisterReply;
using chirp::ChirpRequest;
using chirp::ChirpReply;
using chirp::FollowRequest;
using chirp::FollowReply;
using chirp::readREquest;
using chirp::readReply;
using chirp::MonitorRequest;
using chirp::MonitorReply;

using chirp::PutRequest;
using chirp::PutReply;
using chirp::GetRequest;
using chirp::GetReply;
using chirp::DeleteRequest;
using chirp::DeleteReply;

class ServiceLayerImpl final : public ServiceLayer::Service {
  Status registeruser(ServerContext* context, const RegisterRequest* request,
                  RegisterReply* reply) override {
    // TODO: register with backend
    return Status::OK;
  }
  Status chirp(ServerContext* context, const ChirpRequest* request,
                  ChirpReply* reply) override {
    // TODO: insert chirp into backend service
    return Status::OK;
  }
  Status follow(ServerContext* context, const FollowRequest* request,
                  FollowReply* reply) override {
    //TODO: allow chirp to follow another user by calling backend service
    return Status::OK;
  }
  Status read(ServerContext* context, const ReadRequest* request,
                  ReadReply* reply) override {
    //TODO: get thread from backend service and return
    return Status::OK;
  }
  Status monitor(ServerContext* context, const MonitorRequest* request,
                  MonitorReply* reply) override {
   //TODO: process user's following list and broadcast chirps
    return Status::OK;
  }
};

void run() {
  std::string server_address("0.0.0.0:50002");
  ServiceLayerImpl service;

  ServerBuilder builder;
  
  // listen on server address
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  
  // Register as synchronous service
  builder.RegisterService(&service);
  
  // assemble server
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Keep running until shutdown signal received
  server->Wait();
}

int main(int argc, char** argv) {
  run();
  return 0;
}