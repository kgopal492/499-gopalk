#include "sl_server.h"

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

Status SL_Server::registeruser(ServerContext* context, const RegisterRequest* request,
                RegisterReply* reply){
   mtx_.lock();
  // get users list from KVS, check if username is taken before adding to list
  std::string users_serial = client_.get("users");
  Users users;
  users.ParseFromString(users_serial);
  for(int i = 0; i < users.username_size(); i++) {
    if(users.username(i) == request->username()) {
      mtx_.unlock();
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

  following.SerializeToString(&following_serial);
  followers.SerializeToString(&followers_serial);

  // put lists of followers and following back in database
  client_.put("following", following_serial);
  client_.put("followers", followers_serial);

  // Set monitor key to -1 to indicate monitoring is not occuring yet
  client_.put("monitor::" + request->username(), "-1");
  mtx_.unlock();

  return Status::OK;
}

Status SL_Server::chirp(ServerContext* context, const ChirpRequest* request,
                ChirpReply* reply){
  mtx_.lock();

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
    mtx_.unlock();
    return Status(StatusCode::INVALID_ARGUMENT, "user does not exist");
  }

  // get chirps from database
  std::string chirps_serial = client_.get("chirps");
  Chirps chirps;
  chirps.ParseFromString(chirps_serial);

  // check that parent_id exists in database
  try {
    if( (request->parent_id() != "-1") && ((std::stoi(request->parent_id()) >= chirps.chirps_size()) || (std::stoi(request->parent_id()) < 0)) ) {
      mtx_.unlock();
      return Status(StatusCode::INVALID_ARGUMENT, "parent_id not valid");
    }
  } catch (std::invalid_argument&) {
    mtx_.unlock();
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

  // add new chirp to chirps repeated variable
  Chirp *newChirp = chirps.add_chirps();
  *newChirp = *chirp;

  // add chirp to KVS
  chirps.SerializeToString(&chirps_serial);
  client_.put("chirps", chirps_serial);

  std::string replies_serial = client_.get("replies");
  Replies replies;
  replies.ParseFromString(replies_serial);
  Reply* chirpReply = new Reply();
  chirpReply->set_id(std::stoi(chirp->id()));
  Reply* newReply = replies.add_allreplies();
  *newReply = *chirpReply;

  // add reply to original chirp id if reply flag specified
  if((request->parent_id() != "-1")) {
    replies.mutable_allreplies(std::stoi(request->parent_id()))
        ->add_replies(std::stoi(chirp->id()));
  }

  replies.SerializeToString(&replies_serial);
  client_.put("replies", replies_serial);

  // broadcast chirp to followers
  std::string followers_serial = client_.get("followers");
  Followers followers;
  followers.ParseFromString(followers_serial);

  // loop through each follower
  for(int i = 0; i < followers.followers_size(); i++) {
    if((followers.followers(i)).username() == request->username()) {
      for(int j = 0; j < followers.followers(i).follows_size(); j++) {
        std::string follower_name =
            "monitor::" + followers.followers(i).follows(j);
        std::string monitor_serial = client_.get(follower_name);

        // if monitoring is true, add chirp to broadcast list for user
        if (monitor_serial != "-1") {
          Chirps chirps;
          chirps.ParseFromString(monitor_serial);
          Chirp* monitor_chirp = chirps.add_chirps();
          *monitor_chirp = *chirp;
          chirps.SerializeToString(&monitor_serial);
          client_.put(follower_name, monitor_serial);
        }
      }
    }
  }
  mtx_.unlock();
  return Status::OK;
}

Status SL_Server::follow(ServerContext* context, const FollowRequest* request,
                FollowReply* reply){
  // check that user and to_follow are valid users_
  mtx_.lock();

  // check that request->user() and request->to_follow() are valid
  bool valid_user = false;
  bool valid_to_follow = false;
  std::string users_serial = client_.get("users");
  Users users;
  users.ParseFromString(users_serial);
  for(int i = 0; i < users.username_size(); i++) {
    if(users.username(i) == request->username()) {
      valid_user = true;
    }
    if(users.username(i) == request->to_follow()) {
      valid_to_follow = true;
    }
    if(valid_user && valid_to_follow) {
      break;
    }
  }
  if(!valid_user || !valid_to_follow) {
    mtx_.unlock();
    return Status(StatusCode::INVALID_ARGUMENT, "at least one username provided does not exist");
  }

  // add user and to_follow to followers_ and following_ arrays
  std::string following_serial = client_.get("following");
  std::string followers_serial = client_.get("followers");

  Following following;
  Followers followers;

  following.ParseFromString(following_serial);
  followers.ParseFromString(followers_serial);

  for(int i = 0; i < following.following_size(); i++) {
    if((following.following(i)).username() == request->username()) {
      (following.mutable_following(i))->add_follows(request->to_follow());
    }
  }

  for(int i = 0; i < followers.followers_size(); i++) {
    if((followers.followers(i)).username() == request->to_follow()) {
      (followers.mutable_followers(i))->add_follows(request->username());
    }
  }

  // put new lists in database
  following.SerializeToString(&following_serial);
  followers.SerializeToString(&followers_serial);

  client_.put("following", following_serial);
  client_.put("followers", followers_serial);
  mtx_.unlock();
  return Status::OK;
}

Status SL_Server::read(ServerContext* context, const ReadRequest* request,
                ReadReply* reply){
  mtx_.lock();

  // check if valid chirp id is provided
  std::string chirps_serial = client_.get("chirps");
  Chirps chirps;
  chirps.ParseFromString(chirps_serial);
  try {
    if((std::stoi(request->chirp_id()) < 0) || (std::stoi(request->chirp_id()) >= chirps.chirps_size())) {
      mtx_.unlock();
      return Status(StatusCode::INVALID_ARGUMENT, "invalid chirp id");
    }
  } catch (std::invalid_argument&) {
    mtx_.unlock();
    return Status(StatusCode::INVALID_ARGUMENT, "invalid chirp id");
  }

  // get all replies from database
  std::string replies_serial = client_.get("replies");
  Replies replies;
  replies.ParseFromString(replies_serial);

  // convert replies to vector of vector
  std::vector<std::vector<int> > replies_vec;
  for(int i = 0; i < replies.allreplies_size(); i++) {
    std::vector<int> chirp_replies;
    for(int j = 0; j < replies.allreplies(i).replies_size(); j++) {
      chirp_replies.push_back(replies.allreplies(i).replies(j));
    }
    replies_vec.push_back(chirp_replies);
  }
  //implement DFS to display all read chirps
  std::vector<bool> visited(chirps.chirps().size(), false);
  std::stack<int> dfs_stack;
  dfs_stack.push(std::stoi(request->chirp_id()));
  Chirp *chirp = reply->add_chirps();
  *chirp = chirps.chirps(std::stoi(request->chirp_id()));
  visited[std::stoi(request->chirp_id())] = true;
  while(!dfs_stack.empty()) {
    int curr_id = dfs_stack.top();
    bool child_exists = false;
    for(int i = 0; i < replies_vec[curr_id].size(); i++){
      int reply_id = replies_vec[curr_id][i];
      if(!visited[reply_id]) {
        Chirp *chirpReply = reply->add_chirps();
        *chirpReply = chirps.chirps(reply_id);
        visited[reply_id] = true;
        dfs_stack.push(reply_id);
        child_exists = true;
        break;
      }
    }
    if(!child_exists) {
      dfs_stack.pop();
    }
  }

  mtx_.unlock();
  return Status::OK;
}

Status SL_Server::monitor(ServerContext* context, const MonitorRequest* request, ServerWriter<MonitorReply>* writer){
  // find list of chirps to stream from
  std::string monitor_key = "monitor::" + request->username();
  while(!context->IsCancelled()) {
    mtx_.lock();
    std::string monitor_serial = client_.get(monitor_key);
    Chirps chirps;
    if (monitor_serial != "-1") {
      chirps.ParseFromString(monitor_serial);
    }
    for (int i = 0; i < chirps.chirps_size(); i++) {
      // add each chirp to stream
      MonitorReply monitor_reply;
      Chirp *chirp = new Chirp();
      *chirp = chirps.chirps(i);
      monitor_reply.set_allocated_chirp(chirp);
      writer->Write(monitor_reply);
    }
    // clear chirps in monitor before sending back to KVS
    Chirps emptyChirps;
    emptyChirps.SerializeToString(&monitor_serial);
    client_.put(monitor_key, monitor_serial);
    mtx_.unlock();
  }
  client_.put(monitor_key, "-1");
  return Status::OK;

}
