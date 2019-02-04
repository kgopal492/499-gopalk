#include "backendclient.h"
#include "KeyValueStore.grpc.pb.h"
#include "ServiceLayer.grpc.pb.h"
#include "servicelayerimpl.h"

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
using chirp::ReadRequest;
using chirp::ReadReply;
using chirp::MonitorRequest;
using chirp::MonitorReply;

using chirp::PutRequest;
using chirp::PutReply;
using chirp::GetRequest;
using chirp::GetReply;
using chirp::DeleteRequest;
using chirp::DeleteReply;

// Service Layer's execution (receives requests from command line)
// and creates requests to key-value store
void run() {
  // run server on localhost:50002
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
