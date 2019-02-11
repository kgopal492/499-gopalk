#include "sl_server.h"

#include <stack>
#include <vector>

#include <google/protobuf/util/time_util.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
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

// construct SL_Server with KVS_Client and initialize KVS
SL_Server::SL_Server() : client_(grpc::CreateChannel("localhost:50000", grpc::InsecureChannelCredentials())) {
  // create empty Users object to store list of users
  Users users;
  std::string users_serial;
  users.SerializeToString(&users_serial);

  // Create empty Chirps message to store all chirps
  Chirps chirps;
  std::string chirps_serial;
  chirps.SerializeToString(&chirps_serial);

  // Create replies object to store chirp id to replies
  Replies replies;
  std::string replies_serial;
  replies.SerializeToString(&replies_serial);

  // create following map to map username to list of following
  FollowingMap following;
  std::string following_serial;
  following.SerializeToString(&following_serial);

  // store empty data structures in key value store
  client_.put("users", users_serial);
  client_.put("chirps", chirps_serial);
  client_.put("replies", replies_serial);
  client_.put("following", following_serial);
}


// register user with backend service
Status SL_Server::registeruser(ServerContext* context, const RegisterRequest* request,
                RegisterReply* reply){
  // TODO: register with backend
  // TODO: serialize users, followers, following,  and send to key value store
  // determine if username has been taken
  string users_serial = client_.get("users");
  Users users =
  if(users_.find(request->username()) == users_.end()) {
    users_.insert(request->username());
    std::unordered_set<std::string> empty_set;
    followers_.insert(std::pair<std::string, std::unordered_set<std::string> >(request->username(), empty_set));
    following_.insert(std::pair<std::string, std::unordered_set<std::string> >(request->username(), empty_set));
    return Status::OK;
  }
  else {
    std::cout << "User is already in the database" << std::endl;
    return Status(StatusCode::ALREADY_EXISTS, "username has already been taken");
  }
}

// allow user to send chirp and register with backend
Status SL_Server::chirp(ServerContext* context, const ChirpRequest* request,
                ChirpReply* reply){
  // TODO: insert chirp into backend service
  // TODO: serialize chirps and replies and send to backend
  if(users_.find(request->username()) == users_.end()) {
    return Status(StatusCode::INVALID_ARGUMENT, "user does not exist");
  }
  if( (request->parent_id() != "-1") && (std::stoi(request->parent_id()) >= chirps_.size()) ) {
    return Status(StatusCode::INVALID_ARGUMENT, "parent_id not valid");
  }
  // create new chirp
  Chirp *chirp = new Chirp();

  // create timestamp for chirp
  int64_t seconds = google::protobuf::util::TimeUtil::TimestampToSeconds(google::protobuf::util::TimeUtil::GetCurrentTime());
  int64_t useconds = google::protobuf::util::TimeUtil::TimestampToMicroseconds(google::protobuf::util::TimeUtil::GetCurrentTime());
  chirp::Timestamp* ts = new chirp::Timestamp();
  ts->set_seconds(seconds);
  ts->set_useconds(useconds);

  // set variables of chirp
  chirp->set_allocated_timestamp(ts);
  chirp->set_username(request->username());
  chirp->set_text(request->text());
  chirp->set_id(std::to_string(chirps_.size()));
  chirp->set_parent_id(request->parent_id());
  reply->set_allocated_chirp(chirp);
  chirps_.push_back(*chirp);

  // create empty vector of replies_ for chirp
  std::vector<int> reply_vector;
  replies_.push_back(reply_vector);

  // add reply to original chirp id if reply flag specified
  if((request->parent_id() != "-1")) {
    replies_[std::stoi(request->parent_id())].push_back(chirps_.size()-1);
  }
  return Status::OK;
}

// allow user to follow another user (store in backend)
Status SL_Server::follow(ServerContext* context, const FollowRequest* request,
                FollowReply* reply){
  //TODO: allow chirp to follow another user by calling backend service
  // check that user and to_follow are valid users_
  if((users_.find(request->username()) == users_.end()) || (users_.find(request->to_follow()) == users_.end())) {
    return Status(StatusCode::INVALID_ARGUMENT, "one of the usernames provided is invalid");
  }

  // add user and to_follow to followers_ and following_ arrays
  following_[request->username()].insert(request->to_follow());
  followers_[request->to_follow()].insert(request->username());
  return Status::OK;
}

// allow user to read a thread
Status SL_Server::read(ServerContext* context, const ReadRequest* request,
                ReadReply* reply){
  // TODO: get thread from backend service and return
  // TODO: remove output chirps in service layer and store in reply

  //check if valid chirp id is provided
  if((std::stoi(request->chirp_id()) >= chirps_.size()) || (std::stoi(request->chirp_id()) < 0) ) {
    return Status(StatusCode::INVALID_ARGUMENT, "chirp id provided is invalid");
  }

  //implement DFS to display all read chirps_
  std::vector<bool> visited(chirps_.size(), false);
  std::stack<int> dfs_stack;
  dfs_stack.push(std::stoi(request->chirp_id()));
  std::cout << chirps_[std::stoi(request->chirp_id())].text() << std::endl;
  visited[std::stoi(request->chirp_id())] = true;
  int tabs = 0; // every reply is tabbed in
  while(!dfs_stack.empty()) {
    int curr_id = dfs_stack.top();
    for(int i = 0; i < replies_[curr_id].size(); i++){
      int reply = replies_[curr_id][i];
      if(!visited[reply]) {
	tabs++;
	for(int j = 0; j<tabs; j++) {
	  std::cout << "\t";
	}
	std::cout << chirps_[reply].text() << std::endl;
	visited[reply] = true;
	dfs_stack.push(reply);
	continue;
      }
    }
    dfs_stack.pop();
    tabs--;
  }
  return Status::OK;
}

// allow user to monitor followers
Status SL_Server::monitor(ServerContext* context, const MonitorRequest* request,
                MonitorReply* reply){
 //TODO: process user's following_ list and broadcast chirps_
  return Status::OK;
}
