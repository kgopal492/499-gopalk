#include "sl_functionality.h"


ServiceLayerFunctionality::ServiceLayerFunctionality(const bool& testing) {
  if (testing) {
    client_ = new KeyValueClientTest();
  } else {
    client_ = new KeyValueClient(grpc::CreateChannel("localhost:50000", grpc::InsecureChannelCredentials()));
  }
}

bool ServiceLayerFunctionality::registeruser(const std::string &username) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  const std::string kUserKey = "user::" + username;
  if (user_exists(username)) {
    return false;
  }
  client_->put(kUserKey, "1");
  const std::string kMonitorKey = "monitor::" + username;
  client_->put(kMonitorKey, "0");
  return true;
}

Chirp* ServiceLayerFunctionality::chirp(Chirp* chirp, const std::string& username, const std::string& text, const std::string& parent_id) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  // get new chirp id, and update chirp count
  std::string chirp_id = chirp_count();
  increment_chirp_count();
  // create new chirp and add to database
  create_chirp(chirp, username, text, parent_id, chirp_id);
  add_reply(chirp_id, parent_id);
  // broadcast chirp to followers
  broadcast_chirp(username, *chirp);
  return chirp;
}

// allow user to follow another user (store in backend)
int ServiceLayerFunctionality::follow(const std::string& username, const std::string& to_follow) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  if (!user_exists(username)) {
    return 1; // TODO: change to ENUM
  } else if (!user_exists(to_follow)) {
    return 2;
  }
  const std::string kFollowerKey = "followers::" + to_follow;

  std::string followers_serial = client_->get(kFollowerKey);
  Followers followers;
  followers.ParseFromString(followers_serial);

  for(int i = 0; i < followers.username_size(); i++) {
    if (followers.username(i) == username) {
      return 0;
    }
  }

  followers.add_username(username);

  // put new list in database
  followers.SerializeToString(&followers_serial);

  client_->put(kFollowerKey, followers_serial);
  return 0;
}

std::vector<Chirp> ServiceLayerFunctionality::read(const std::string& chirp_id) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  std::vector<Chirp> chirps;
  read_thread(chirp_id, &chirps);
  return chirps;
}
// return the current chirps broadcast to a user that is monitoring
Chirps ServiceLayerFunctionality::monitor(const std::string& username) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  const std::string kMonitorKey = "monitor::" + username;
  std::string monitor_serial = client_->get(kMonitorKey);
  Chirps chirps;
  if ((monitor_serial != "0") && (monitor_serial != "1")) {
    chirps.ParseFromString(monitor_serial);
  }
  client_->put(kMonitorKey, "1");
  return chirps;
}

bool ServiceLayerFunctionality::user_exists(const std::string &username) {
  const std::string kUserKey = "user::" + username;
  std::string users_serial = client_->get(kUserKey);
  if (users_serial == "1") {
    return true;
  }
  return false;
}

// check if parent_id is valid
bool ServiceLayerFunctionality::valid_parent_id(const std::string& parent_id) {
  try {
    if ( (parent_id != "-1") && ((std::stoi(parent_id) >= std::stoi(chirp_count())) || (std::stoi(parent_id) < 0))) {
      return false;
    }
  } catch (std::invalid_argument&) {
    return false;
  }
  return true;
}

// create chirp to be inserted into key value store
void ServiceLayerFunctionality::create_chirp(Chirp* chirp, const std::string& username, const std::string& text, const std::string& parent_id, const std::string& chirp_id) {
  // create timestamp for chirp
  int64_t seconds = google::protobuf::util::TimeUtil::TimestampToSeconds(google::protobuf::util::TimeUtil::GetCurrentTime());
  int64_t useconds = google::protobuf::util::TimeUtil::TimestampToMicroseconds(google::protobuf::util::TimeUtil::GetCurrentTime());

  // set member variables of Chirp
  chirp::Timestamp* ts = chirp->mutable_timestamp();
  ts->set_seconds(seconds);
  ts->set_useconds(useconds);
  chirp->set_username(username);
  chirp->set_text(text);
  chirp->set_id(chirp_id);
  chirp->set_parent_id(parent_id);

  // serialize chirp and add to key value store
  const std::string kChirpKey = "chirp::" + chirp_id;
  std::string chirp_serial;
  chirp->SerializeToString(&chirp_serial);
  client_->put(kChirpKey, chirp_serial);
}

// add reply to parent chirp
void ServiceLayerFunctionality::add_reply(const std::string& chirp_id, const std::string& parent_id){
  const std::string kReplyParentKey = "reply::" + parent_id;
  std::string replies_serial = client_->get(kReplyParentKey);
  Replies replies;
  replies.ParseFromString(replies_serial);
  replies.add_id(chirp_id);
  replies.SerializeToString(&replies_serial);
  client_->put(kReplyParentKey, replies_serial);
  return;
}

// return current total count of chirps
std::string ServiceLayerFunctionality::chirp_count() {
  const std::string kChirpCountKey = "chirp_count::";
  std::string chirp_count_serial = client_->get(kChirpCountKey);
  if (chirp_count_serial.empty()) {
    return "0";
  }
  return chirp_count_serial;
}
// add one to total chirp count
void ServiceLayerFunctionality::increment_chirp_count() {
  const std::string kChirpCountKey = "chirp_count::";
  std::string chirp_count_serial = client_->get(kChirpCountKey);
  int count = 0;
  if (!chirp_count_serial.empty()) {
    count = std::stoi(chirp_count_serial);
  }
  count++;
  chirp_count_serial = std::to_string(count);
  client_->put(kChirpCountKey, chirp_count_serial);
}

void ServiceLayerFunctionality::broadcast_chirp(const std::string& username, Chirp chirp) {
  const std::string kFollowersKey = "followers::" + username;
  std::string followers_serial = client_->get(kFollowersKey);
  Followers followers;
  followers.ParseFromString(followers_serial);

  // loop through each follower
  for(int i = 0; i < followers.username_size(); i++) {
    std::string follower_key ="monitor::" + followers.username(i);
    std::string monitor_serial = client_->get(follower_key);

    // if monitoring is true, add chirp to broadcast list for user
    if (monitor_serial != "0") {
      Chirps chirps;
      chirps.ParseFromString(monitor_serial);
      Chirp* monitor_chirp = chirps.add_chirps();
      *monitor_chirp = chirp;
      chirps.SerializeToString(&monitor_serial);
      client_->put(follower_key, monitor_serial);
    }
  }
}

void ServiceLayerFunctionality::read_thread(const std::string& chirp_id, std::vector<Chirp>* chirps) {
  // push back current chirp to vector
  const std::string kChirpKey = "chirp::" + chirp_id;
  std::string chirp_serial = client_->get(kChirpKey);
  if (!chirp_serial.empty()) {
    Chirp chirp;
    chirp.ParseFromString(chirp_serial);
    chirps->push_back(chirp);
    std::string kReplyParentKey = "reply::" + chirp_id;
    std::string replies_serial = client_->get(kReplyParentKey);
    Replies replies;
    replies.ParseFromString(replies_serial);
    for(int i = 0; i < replies.id_size(); i++) {
      read_thread(replies.id(i), chirps);
    }
  }
}

void ServiceLayerFunctionality::clear_monitor(const std::string& username) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  const std::string kMonitorKey = "monitor::" + username;
  client_->put(kMonitorKey, "0");
}
