#include "servicelayerimpl.h"

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

// register user with backend service
Status ServiceLayerImpl::registeruser(ServerContext* context, const RegisterRequest* request,
                RegisterReply* reply){
  // TODO: register with backend
  
  // determine if username is 
  if(users.find(request->username()) == users.end()) {
    users.insert(request->username());
    std::unordered_set<std::string> new_followers;
    std::unordered_set<std::string> new_following;
    followers.insert(std::pair<std::string, std::unordered_set<std::string> >(request->username(), new_followers));
    following.insert(std::pair<std::string, std::unordered_set<std::string> >(request->username(), new_following));
    return Status::OK;
  }
  else {
    std::cout << "User is already in the database" << std::endl;
    return Status(StatusCode::ALREADY_EXISTS, "username has already been taken");
  }
}

// allow user to send chirp and register with backend
Status ServiceLayerImpl::chirp(ServerContext* context, const ChirpRequest* request,
                ChirpReply* reply){
  // TODO: insert chirp into backend service
  if(users.find(request->username()) == users.end()) {
    return Status(StatusCode::INVALID_ARGUMENT, "user does not exist");
  }
  if( (request->parent_id() != "-1") && (std::stoi(request->parent_id()) >= chirps.size()) ) {
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
  chirp->set_id(std::to_string(chirps.size()));
  chirp->set_parent_id(request->parent_id());
  reply->set_allocated_chirp(chirp);
  chirps.push_back(*chirp);
  
  // create empty vector of replies for chirp
  std::vector<int> reply_vector;
  replies.push_back(reply_vector);

  // add reply to original chirp id if reply flag specified
  if((request->parent_id() != "-1")) {
    replies[std::stoi(request->parent_id())].push_back(chirps.size()-1);
  }
  return Status::OK;
}

// allow user to follow another user (store in backend)
Status ServiceLayerImpl::follow(ServerContext* context, const FollowRequest* request,
                FollowReply* reply){
  //TODO: allow chirp to follow another user by calling backend service
  
  // check that user and to_follow are valid users
  if((users.find(request->username()) == users.end()) || (users.find(request->to_follow()) == users.end())) {
    return Status(StatusCode::INVALID_ARGUMENT, "one of the usernames provided is invalid");
  }

  // add user and to_follow to followers and following arrays
  following[request->username()].insert(request->to_follow());
  followers[request->to_follow()].insert(request->username());
  return Status::OK;
}

// allow user to read a thread
Status ServiceLayerImpl::read(ServerContext* context, const ReadRequest* request,
                ReadReply* reply){
  //TODO: get thread from backend service and return
 
  //check if valid chirp id is provided
  if((std::stoi(request->chirp_id()) >= chirps.size()) || (std::stoi(request->chirp_id()) < 0) ) {
    return Status(StatusCode::INVALID_ARGUMENT, "chirp id provided is invalid");
  }
  
  //implement DFS to display all read chirps
  std::vector<bool> visited(chirps.size(), false);
  std::stack<int> dfs_stack;
  dfs_stack.push(std::stoi(request->chirp_id()));
  std::cout << chirps[std::stoi(request->chirp_id())].text() << std::endl;
  visited[std::stoi(request->chirp_id())] = true;
  int tabs = 0; // every reply is tabbed in
  while(!dfs_stack.empty()) {
    int curr_id = dfs_stack.top();
    for(int i = 0; i < replies[curr_id].size(); i++){
      int reply = replies[curr_id][i];
      if(!visited[reply]) {
	tabs++;
	for(int j = 0; j<tabs; j++) {
	  std::cout << "\t";
	}
	std::cout << chirps[reply].text() << std::endl;
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
Status ServiceLayerImpl::monitor(ServerContext* context, const MonitorRequest* request,
                MonitorReply* reply){
 //TODO: process user's following list and broadcast chirps
  return Status::OK;
}
