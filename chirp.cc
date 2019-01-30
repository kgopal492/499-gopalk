#include "ServiceLayer.grpc.pb.h"

#include <iostream>
#include <memory>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::ClientContext;
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
using chirp::ServiceLayer;

// Define flags for gflags
DEFINE_string(register, "", "Registers the given username");
DEFINE_string(user, "", "Logs in as the given username");
DEFINE_string(chirp, "", "Creates a new chirp with the given text");
DEFINE_string(reply, "", "Indicates that the new chirp is a reply to the given id");
DEFINE_string(follow, "", "Starts following the given username");
DEFINE_string(read, "", "Reads the chirp thread starting at the given id");
DEFINE_bool(monitor, false, "Streams new tweets from those currently followed");

class ChirpClient {
 public:
  ChirpClient(std::shared_ptr<Channel> channel)
      : stub_(ServiceLayer::NewStub(channel)) {}

  // - registers username of new user with key-value store
  // - returns true if username is available
  bool registeruser(const std::string& username) {
    // Send username to the service layer
    RegisterRequest request;
    request.set_username(username);

    // Store response in reply
    RegisterReply reply;

    // Client's context for additional information to server, or rpc options
    ClientContext context;

    // Send the rpc
    Status status = stub_->registeruser(&context, request, &reply);

    // Determine if the status is ok, then process
    if (status.ok()) {
      return true;
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return false;
    }
  }
  
  // - logs in user to command line
  // - returns true if user exists (and login is valid)
  Chirp chirp(const std::string& username, std::string& text, const char* &parent_id) {
    ChirpRequest request;
    request.set_username(username);
    request.set_text(text);
    request.set_parent_id(parent_id);

    ChirpReply reply;

    ClientContext context;

    Status status = stub_->chirp(&context, request, &reply);

    if (status.ok()) {
      return reply.chirp();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      Chirp chirp; // TODO: return null/invalid value
      return chirp;
    }
  }

  // - takes text to be chirped, and sends to key-value store
  // - returns true if chirp is successfully
  //   registered with key-value store
  bool follow(const std::string& username, const std::string& to_follow) {
    // Send username of client and username of chirp user to follow
    FollowRequest request;
    request.set_username(username);
    request.set_to_follow(to_follow);

    FollowReply reply;

    ClientContext context;

    Status status = stub_->follow(&context, request, &reply);

    if (status.ok()) {
      return true;
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return false;
    }
  }

  // - takes chirpID of beginning of thread
  // - returns string of chirp thread
  Chirp read(const char* &chirp_id) {
    ReadRequest request;
    request.set_chirp_id(chirp_id);

    ReadReply reply;

    ClientContext context;

    Status status = stub_->read(&context, request, &reply);

    if (status.ok()) {
      return reply.chirps();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      Chirp chirp; // TODO: return null/invalid value
      return chirp;
    }
  }

  // - waits for service layer to send chirps of following users
  const google::protobuf::RepeatedPtrField<chirp::Chirp> monitor(const std::string& username) {
    MonitorRequest request;
    request.set_username(username);

    MonitorReply reply;

    ClientContext context;

    Status status = stub_->monitor(&context, request, &reply);

    if (status.ok()) {
      return reply.chirp();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      Chirp chirp; // TODO: return null/invalid value
      return chirp;
    }
  }

 private:
  std::unique_ptr<ServiceLayer::Stub> stub_;
};

int main(int argc, char *argv[]) {
  // --user flag is required for each action except register
  // TODO: ensure required flags are present
  // TODO: call functions from Service class based on flags


  ChirpClient client(grpc::CreateChannel(
      "localhost:50002", grpc::InsecureChannelCredentials()));
}
