#include "sl_server.h"

#include <stack>
#include <stdexcept>
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
  Following following;
  std::string following_serial;
  following.SerializeToString(&following_serial);

  // create follower list to map username to followers
  Followers followers;
  std::string followers_serial;
  followers.SerializeToString(&followers_serial);
  // store empty data structures in key value store
  client_.put("users", users_serial);
  client_.put("chirps", chirps_serial);
  client_.put("replies", replies_serial);
  client_.put("following", following_serial);
  client_.put("followers", followers_serial);
}


// register user with backend service
Status SL_Server::registeruser(ServerContext* context, const RegisterRequest* request,
                RegisterReply* reply){

  // get users list from KVS, check if username is taken before adding to list
  std::string users_serial = client_.get("users");
  Users users;
  users.ParseFromString(users_serial);
  for(int i = 0; i < users.username_size(); i++) {
    if(users.username(i) == request->username()) {
      return Status(StatusCode::ALREADY_EXISTS, "username has already been taken");
    }
  }

  // put `users` back in database
  users.add_username(request->username());
  users.SerializeToString(&users_serial);
  client_.put("users", users_serial);

  // Create empty list of followers and following for given user
  Follow follow;
  follow.set_username(request->username());
  std::string following_serial = client_.get("following");
  std::string followers_serial = client_.get("followers");

  Following following;
  Followers followers;

  following.ParseFromString(following_serial);
  followers.ParseFromString(followers_serial);

  Follow* newFollowing = following.add_following();
  Follow* newFollower = followers.add_followers();

  newFollowing->set_username(request->username());
  newFollower->set_username(request->username());

  following.add_following(newFollowing);
  followers.add_following(newFollowers);

  following.SerializeToString(&following_serial);
  followers.SerializeToString(&followers_serial);

  client_.put("following", following_serial);
  client_.put("followers", followers_serial);

  std::cout << following_serial << std::endl;
  std::cout << followers_serial << std::endl;
  return Status::OK;
}

// allow user to send chirp and register with backend
Status SL_Server::chirp(ServerContext* context, const ChirpRequest* request,
                ChirpReply* reply){
  // TODO: insert chirp into backend service
  // TODO: serialize chirps and replies and send to backend
  // TODO: make sure atoi works for parent_id
  // TODO: check if text is empty

  // check that user exists in database
  bool valid_user = false;
  std::string users_serial = client_.get("users");
  Users users;
  users.ParseFromString(users_serial);
  for(int i = 0; i < users.username_size(); i++) {
    if(users.username(i) == request->username()) {
      valid_user = true;
      break;
    }
  }
  if(!valid_user) {
    return Status(StatusCode::INVALID_ARGUMENT, "user does not exist");
  }

  // get chirps from database
  std::string chirps_serial = client_.get("chirps");
  Chirps chirps;
  chirps.ParseFromString(chirps_serial);

  // check that parent_id exists in database
  try {
    if( (request->parent_id() != "-1") && ((std::stoi(request->parent_id()) >= chirps.chirps_size()) || (std::stoi(request->parent_id()) < 0)) ) {
      return Status(StatusCode::INVALID_ARGUMENT, "parent_id not valid");
    }
  } catch (std::invalid_argument&) {
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
  chirp->set_id(std::to_string(chirps.chirps_size()));
  chirp->set_parent_id(request->parent_id());
  reply->set_allocated_chirp(chirp);

  // add chirp to KVS
  chirps.add_chirps(*chirp);
  chirps.SerializeToString(&chirps_serial);
  client_.put("chirps", chirps_serial);

  // add reply to original chirp id if reply flag specified
  if((request->parent_id() != "-1")) {
    std::string replies_serial = client_.get("replies");
    Replies replies;
    replies.ParseFromString(replies_serial);
    Reply reply;
    reply.set_username(request->username());
    replies.add_allReplies(reply);
    replies.SerializeToString(&replies_serial);
    client_.put("replies", replies_serial);
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
