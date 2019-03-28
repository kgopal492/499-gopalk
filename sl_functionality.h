#include "Backend.grpc.pb.h"
#include "Backend.pb.h"
#include "KeyValueStore.grpc.pb.h"
#include "KeyValueStore.pb.h"
#include "ServiceLayer.grpc.pb.h"
#include "ServiceLayer.pb.h"
#include "kvs_client.h"
#include "kvs_client_test.h"

#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <google/protobuf/util/time_util.h>
#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;
using grpc::StatusCode;

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

using chirp::Chirps;
using chirp::Replies;
using chirp::Followers;

#ifndef CHIRP_SL_FUNCTIONALITY_H
#define CHIRP_SL_FUNCTIONALITY_H

// implementation of service layer
// takes request from command line clients
class SL_Functionality{
 public:
  // constructor, initializes KVS_Client
  SL_Functionality(const bool &testing);
  // register user with backend service
  bool registeruser(const std::string &username);
  // allow user to send chirp and register with backend
  Chirp* chirp(const std::string& username, const std::string& text, const std::string& parent_id);
  // allow user to follow another user (store in backend)
  int follow(const std::string& username, const std::string& to_follow);
  // return a vector of Chirps to read a thread of a chirp and its replies
  // if vector is empty, chirp_id is invalid
  std::vector<Chirp> read(const std::string& chirp_id);
  // return the current chirps broadcast to a user that is monitoring
  Chirps monitor(const std::string& username);
  // clears monitor value so that monitored chirps are no longer broadcast to the follower
  void clear_monitor(const std::string& username);
  // check if user exists in key value store backend
  bool user_exists(const std::string& username);
  // check if parent_id is valid
  bool valid_parent_id(const std::string& parent_id);

 private:
  // create chirp to be inserted into key value store
  Chirp* create_chirp(const std::string& username, const std::string& text, const std::string& parent_id, const std::string& chirp_id);
  // add reply to parent chirp
  void add_reply(const std::string& chirp_id, const std::string& parent_id);
  // return current total count of chirps
  std::string chirp_count();
  // recursively read all chirps in a thread
  void read_thread(const std::string& chirp_id, std::vector<Chirp>* chirps);
  // add one to total chirp count
  void increment_chirp_count();
  // broadcast a chirp to all followers of the user who are monitoring
  void broadcast_chirp(const std::string& username, Chirp chirp);
  // client for keyvaluestore layer
  KVS_Client_Interface* client_;
  // mutex to lock sl for monitor and other functions
  std::mutex sl_func_mtx_;
};

#endif // CHIRP_SL_FUNCTIONALITY_H
