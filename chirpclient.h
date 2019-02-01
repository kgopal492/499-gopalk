#include <iostream>
#include <memory>
#include <string>

#include <glog/logging.h>
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

#ifndef CHIRP_BACKENDCLIENT_H
#define CHIRP_BACKENDCLIENT_H

// interface to send grpc request to the service layer
// from the chirp client's command line
class ChirpClient {
 public:
  ChirpClient(std::shared_ptr<Channel> channel)
      : stub_(ServiceLayer::NewStub(channel)) {}
  // - registers username of new user with key-value store
  // - returns true if username is available
  bool registeruser(const std::string& username);
  // - logs in user to command line
  // - returns true if user exists (and login is valid)
  Chirp chirp(const std::string& username, std::string& text, const std::string& parent_id);
  // - takes text to be chirped, and sends to key-value store
  // - returns true if chirp is successfully
  //   registered with key-value store
  bool follow(const std::string& username, const std::string& to_follow);
  // - takes chirpID of beginning of thread
  // - returns string of chirp thread
  const google::protobuf::RepeatedPtrField<chirp::Chirp> read(const std::string& chirp_id);
  // - waits for service layer to send chirps of following users
  void monitor(const std::string& username);

 private:
  std::unique_ptr<ServiceLayer::Stub> stub_;
};

#endif