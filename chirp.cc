#include "sl_client.h"

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
DEFINE_string(reply, "-1", "Indicates that the new chirp is a reply to the given id");
DEFINE_string(follow, "", "Starts following the given username");
DEFINE_string(read, "", "Reads the chirp thread starting at the given id");
DEFINE_bool(monitor, false, "Streams new tweets from those currently followed");

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  // create connection with service layer
  SL_Client client(grpc::CreateChannel("localhost:50002", grpc::InsecureChannelCredentials()));

  // register flag provided, register username
  if(!FLAGS_register.empty()) {
    if(client.registeruser(FLAGS_register)) {
      std::cout << "Successfully registered user " << FLAGS_register << std::endl;
    } else {
      std::cout << "Attempt to register user " << FLAGS_register << " failed." << std::endl;
      return 1;
    }
  }

  // check if username is provided before performing tasks
  if(!FLAGS_user.empty()) {
    // chirp flag provided, make chirp
    if(!FLAGS_chirp.empty()) {
      // TODO: store Chirp reply message and handle
      client.chirp(FLAGS_user, FLAGS_chirp, FLAGS_reply);
    }

    // follow flag provided, follow given user
    if(!FLAGS_follow.empty()) {
      if(client.follow(FLAGS_user, FLAGS_follow)) {
        std::cout << "User " << FLAGS_user << " successfully followed " << FLAGS_follow << "." << std::endl;
      } else {
        std::cout << "Attempt for " << FLAGS_user << " to follow " << FLAGS_follow << " failed." << std::endl;
        return 1;
      }
    }

    // monitor flag true, stream chirps
    if(FLAGS_monitor) {
      // TODO: handle failure
      client.monitor(FLAGS_user);
    }
  } else if(!FLAGS_chirp.empty() || !FLAGS_follow.empty() || FLAGS_monitor) {
    std::cout << "Cannot complete task without user logged in." << std::endl;
  }

  // read flag provided, read chirp thread
  if(!FLAGS_read.empty()) {
    // TODO: store return in variable, and handle failure
    client.read(FLAGS_read);
  }

  return 0;
}
