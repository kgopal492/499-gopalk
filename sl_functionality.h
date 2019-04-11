#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <google/protobuf/util/time_util.h>
#include <grpcpp/grpcpp.h>
#include "Backend.grpc.pb.h"
#include "Backend.pb.h"
#include "KeyValueStore.grpc.pb.h"
#include "KeyValueStore.pb.h"
#include "ServiceLayer.grpc.pb.h"
#include "ServiceLayer.pb.h"
#include "kvs_client.h"
#include "kvs_client_test.h"

using chirp::Chirp;
using chirp::ChirpReply;
using chirp::ChirpRequest;
using chirp::Chirps;
using chirp::Followers;
using chirp::FollowReply;
using chirp::FollowRequest;
using chirp::MonitorReply;
using chirp::MonitorRequest;
using chirp::ReadReply;
using chirp::ReadRequest;
using chirp::RegisterReply;
using chirp::RegisterRequest;
using chirp::Replies;
using chirp::ServiceLayer;
using chirp::Streamers;

#ifndef CHIRP_SL_FUNCTIONALITY_H
#define CHIRP_SL_FUNCTIONALITY_H

// implementation of service layer
// takes request from command line clients
// accesses key value store to complete requests
class ServiceLayerFunctionality {
 public:
  // constructor, initializes KeyValueClient
  ServiceLayerFunctionality(const bool& testing);
  // register user with backend service
  bool registeruser(const std::string& username);
  // allow user to send chirp and register with backend
  Chirp* chirp(Chirp* chirp, const std::string& username,
               const std::string& text, const std::string& parent_id);
  // allow user to follow another user (store in backend)
  void follow(const std::string& username, const std::string& to_follow);
  // return a vector of Chirps to read a thread of a chirp and its replies
  // if vector is empty, chirp_id is invalid
  std::vector<Chirp> read(const std::string& chirp_id);
  // return the current chirps broadcast to a user that is monitoring
  Chirps monitor(const std::string& username);
  // return the current chirps broadcast to a user that is streaming
  Chirps stream(const std::string& username);
  // adds a user to the stream list for the given hashtag
  void start_stream(const std::string& username, const std::string& hashtag);
  // removes a user from the stream list for the given hashtag
  void end_stream(const std::string& username, const std::string& hashtag);
  // clears monitor value so that monitored chirps are no longer broadcast to
  // the follower
  void clear_monitor(const std::string& username);
  // check if user exists in key value store backend
  bool user_exists(const std::string& username);
  // check if parent_id is valid
  bool valid_parent_id(const std::string& parent_id);

 private:
  // create chirp to be inserted into key value store
  void create_chirp(Chirp* chirp, const std::string& username,
                    const std::string& text, const std::string& parent_id,
                    const std::string& chirp_id);
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
  // checks a chirp for hashtags and returns a vector with all of the hashtags
  // the chirp contains
  std::vector<std::string> chirp_hashtag_check(Chirp chirp);
  // broadcast a chirp to all users who are streaming for a given hashtag
  void stream_chirp(Chirp chirp, const std::string& hashtag);
  // client for keyvaluestore layer
  KeyValueClientInterface* client_;
  // mutex to lock sl for monitor and other functions
  std::mutex sl_func_mtx_;
  // constant to represent true value in database
  const std::string kTrue_ = "[TRUE]";
  // constant to represent false value in database
  const std::string kFalse_ = "[FALSE]";
  // constant to prepend the key that holds the streamed chirps for a user in
  // database
  const std::string kUserStream_ = "stream::";
  // constant to prepend the key that holds the users who are streaming on a
  // given hashtag
  const std::string kHashtagStream_ = "streaming::";
};
#endif  // CHIRP_SL_FUNCTIONALITY_H
