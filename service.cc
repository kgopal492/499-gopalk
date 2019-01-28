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
  // register user with backend service
  Status registeruser(ServerContext* context, const RegisterRequest* request,
                  RegisterReply* reply) override {
    // TODO: register with backend
    return Status::OK;
  }

  // allow user to send chirp and register with backend
  Status chirp(ServerContext* context, const ChirpRequest* request,
                  ChirpReply* reply) override {
    // TODO: insert chirp into backend service
    return Status::OK;
  }

  // allow user to follow another user (store in backend)
  Status follow(ServerContext* context, const FollowRequest* request,
                  FollowReply* reply) override {
    //TODO: allow chirp to follow another user by calling backend service
    return Status::OK;
  }

  // allow user to read a thread
  Status read(ServerContext* context, const ReadRequest* request,
                  ReadReply* reply) override {
    //TODO: get thread from backend service and return
    return Status::OK;
  }

  // allow user to monitor followers
  Status monitor(ServerContext* context, const MonitorRequest* request,
                  MonitorReply* reply) override {
   //TODO: process user's following list and broadcast chirps
    return Status::OK;
  }
};

class BackendClient {
 public:
  BackendClient(std::shared_ptr<Channel> channel)
      : stub_(KeyValueStore::NewStub(channel)) {}

  // take `key` and `value` to insert into key-value table
  // and return whether insertion was successful
  bool put(const int& key, const int& value) {
    // Send key and value to key-value store
    PutRequest request;
    request.set_key(key);
    request.set_value(value);

    // Store response in reply
    PutReply reply;

    // Client's context for additional information to server, or rpc options
    ClientContext context;

    // Send the rpc
    Status status = stub_->put(&context, request, &reply);

    // Determine if the status is ok, then process
    if (status.ok()) {
      return true;
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return false;
    }
  }
  
  // use `key` to return associated values
  std::string get(const std::string& key) {
    GetRequest request;
    request.set_key(key);

    GetReply reply;

    ClientContext context;

    Status status = stub_->get(&context, request, &reply);

    if (status.ok()) {
      return reply.value();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return nullptr;
    }
  }

  // delete key value pair associate with `key` parameter
  bool deletekey(const std::string& key) {
    DeleteRequest request;
    request.set_key(key);

    DeleteReply reply;

    ClientContext context;

    Status status = stub_->deletekey(&context, request, &reply);

    if (status.ok()) {
      return true;
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return false;
    }
  }
};

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