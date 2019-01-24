#include <iostream>
#include <memory>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>
#include "service.h"

// Define flags for gflags
DEFINE_string(register, "", "Registers the given username");
DEFINE_string(user, "", "Logs in as the given username");
DEFINE_string(chirp, "", "Creates a new chirp with the given text");
DEFINE_string(reply, "", "Indicates that the new chirp is a reply to the given id");
DEFINE_string(follow, "", "Starts following the given username");
DEFINE_string(read, "", "Reads the chirp thread starting at the given id");
DEFINE_bool(monitor, false, "Streams new tweets from those currently followed");

int main(int argc, char *argv[]) {
  // --user flag is required for each action except register
  // TODO: ensure required flags are present
  // TODO: call functions from Service class based on flags
}