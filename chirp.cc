#include "chirpclient.h"

#include <iostream>
#include <memory>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

// Define flags for gflags
DEFINE_string(register, "", "Registers the given username");
DEFINE_string(user, "", "Logs in as the given username");
DEFINE_string(chirp, "", "Creates a new chirp with the given text");
DEFINE_string(reply, "", "Indicates that the new chirp is a reply to the given id");
DEFINE_string(follow, "", "Starts following the given username");
DEFINE_string(read, "", "Reads the chirp thread starting at the given id");
DEFINE_bool(monitor, false, "Streams new tweets from those currently followed");

int main(int argc, char *argv[]) {
  // create connection with service layer
  ChirpClient client(grpc::CreateChannel(
    "localhost:50002", grpc::InsecureChannelCredentials()));

  // register flag provided, register username
  if(!FLAGS_register.empty()) {
    if(client.registeruser(FLAGS_register)) {
      std::cout << "Successfully registered user " << FLAGS_register << std::endl;
    }
    else {
      std::cout << "Attempt to register user " << FLAGS_register << " failed." << std::endl;
      return 1;
    }
  }

  // chirp flag provided, make chirp
  if(!FLAGS_chirp.empty()) {
    if(FLAGS_username.empty()) {
      std::cout << "Cannot make chirp without logging in user" << std::endl;
      return 1;
    }

    // TODO: store Chirp reply message and handle
    client.chirp(FLAGS_username, FLAGS_chirp, FLAGS_reply);
  }

  // follow flag provided, follow given user
  if(!FLAGS_follow.empty()) {
    if(FLAGS_username.empty()) {
      std::cout << "Cannot follow without logging in user" << std::endl;
      return 1;
    }
    if(client.follow(FLAGS_username, FLAGS_follow)) {
      std::cout << "User " << FLAGS_username << " successfully followed " << FLAGS_follow << "." << std::endl;
    }
    else {
      std::cout << "Attempt for " << FLAGS_username << " to follow " << FLAGS_follow << " failed." << std::endl;
      return 1;
    }
  }

  // read flag provided, read chirp thread
  if(!FLAGS_read.empty()) {
    // TODO: store return in variable, and handle failure
    client.read(FLAGS_read);
  }

  // monitor flag true, stream chirps
  if(FLAGS_monitor) {
    // TODO: handle failure
    client.monitor();
  }

  return 0;
}