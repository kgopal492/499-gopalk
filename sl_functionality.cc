#include "sl_functionality.h"

ServiceLayerFunctionality::ServiceLayerFunctionality(const bool& testing) {
  // if testing is used, initialize client_ with testing class (doesn't use
  // gRPC)
  if (testing) {
    client_ = new KeyValueClientTest();
  } else {  // Else use KeyValueClient that connects to the KeyValueServer over
            // gRPC
    client_ = new KeyValueClient(grpc::CreateChannel(
        "localhost:50000", grpc::InsecureChannelCredentials()));
  }
}

bool ServiceLayerFunctionality::registeruser(const std::string& username) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  // Form the potential user's key
  const std::string kUserKey = "user::" + username;
  // If key is present, return false since username is taken
  if (user_exists(username)) {
    return false;
  }
  // Set the key as taken
  client_->put(kUserKey, kTrue_);
  // Set monitoring initially to false
  const std::string kMonitorKey = "monitor::" + username;
  client_->put(kMonitorKey, kFalse_);
  return true;
}

Chirp* ServiceLayerFunctionality::chirp(Chirp* chirp,
                                        const std::string& username,
                                        const std::string& text,
                                        const std::string& parent_id) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  // get new chirp id, and update chirp count
  std::string chirp_id = chirp_count();
  increment_chirp_count();
  // create new chirp and add to database
  create_chirp(chirp, username, text, parent_id, chirp_id);
  // add a reply to the chirp for a valid parent id
  add_reply(chirp_id, parent_id);
  // broadcast chirp to followers
  broadcast_chirp(username, *chirp);
  // return chirp to the servicelayer class
  return chirp;
}

// allow user to follow another user (store in backend)
void ServiceLayerFunctionality::follow(const std::string& username,
                                       const std::string& to_follow) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  // Form followers key for the given user
  const std::string kFollowerKey = "followers::" + to_follow;
  // Retrieve followers from the key value store
  std::string followers_serial = client_->get(kFollowerKey);
  Followers followers;
  followers.ParseFromString(followers_serial);
  // Loop through followers and return if user is already following the other
  // user
  for (int i = 0; i < followers.username_size(); i++) {
    if (followers.username(i) == username) {
      return;
    }
  }

  // Add the username as a new follower
  followers.add_username(username);

  // put new list in database
  followers.SerializeToString(&followers_serial);
  client_->put(kFollowerKey, followers_serial);
}

std::vector<Chirp> ServiceLayerFunctionality::read(
    const std::string& chirp_id) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  // Create a vector to store all the chirps in the thread
  std::vector<Chirp> chirps;
  // call recursive function to read all of the chirps
  read_thread(chirp_id, &chirps);
  return chirps;
}
Chirps ServiceLayerFunctionality::monitor(const std::string& username) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  // Form monitor key to retrieve new chirps broadcast to the user
  const std::string kMonitorKey = "monitor::" + username;
  std::string monitor_serial = client_->get(kMonitorKey);
  Chirps chirps;
  // if the monitor has a chirp value, parse the value into a list of chirps
  if ((monitor_serial != kFalse_) && (monitor_serial != kTrue_)) {
    chirps.ParseFromString(monitor_serial);
  }
  // put monitor key back into the array as true
  client_->put(kMonitorKey, kTrue_);
  return chirps;
}

Chirps ServiceLayerFunctionality::stream(const std::string& username) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  // Form stream key to retrieve new chirps broadcast to the user
  const std::string kStreamKey = kUserStream_ + username;
  std::string stream_serial = client_->get(kStreamKey);
  Chirps chirps;
  if (stream_serial == kFalse_) {
    return chirps;
  }
  chirps.ParseFromString(stream_serial);
  // put stream key back into the array as empty
  client_->put(kStreamKey, kFalse_);
  return chirps;
}

void ServiceLayerFunctionality::start_stream(const std::string& username,
                                             const std::string& hashtag) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  if (hashtag.length() == 1 || hashtag.find("#") == std::string::npos) {
    return;
  }
  // Form streaming key for the given hashtag
  const std::string kStreamingKey = kHashtagStream_ + hashtag;
  // Retrieve streamers from the key value store
  std::string streaming_serial = client_->get(kStreamingKey);
  Streamers streamers;
  streamers.ParseFromString(streaming_serial);

  // Add the username as a new streamer
  streamers.add_username(username);

  // put new list in database
  streamers.SerializeToString(&streaming_serial);
  client_->put(kStreamingKey, streaming_serial);
}
void ServiceLayerFunctionality::end_stream(const std::string& username,
                                           const std::string& hashtag) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  // Form streaming key for the given hashtag
  const std::string kStreamingKey = kHashtagStream_ + hashtag;
  // Retrieve streamers from the key value store
  std::string streaming_serial = client_->get(kStreamingKey);
  Streamers streamers;
  streamers.ParseFromString(streaming_serial);

  Streamers updated_streamers;
  for (int i = 0; i < streamers.username_size(); i++) {
    if (streamers.username(i) != username) {
      updated_streamers.add_username(streamers.username(i));
    }
  }

  if (updated_streamers.username_size() != 0) {
    updated_streamers.SerializeToString(&streaming_serial);
    client_->put(kStreamingKey, streaming_serial);
  } else {
    client_->deletekey(kStreamingKey);
  }

  const std::string kStreamKey = kUserStream_ + username;
  client_->put(kStreamKey, kFalse_);
}

bool ServiceLayerFunctionality::user_exists(const std::string& username) {
  // Form users key to request whether user exists
  const std::string kUserKey = "user::" + username;
  std::string users_serial = client_->get(kUserKey);
  // Only if the string contains the true value, user exists
  if (users_serial == kTrue_) {
    return true;
  }
  return false;
}

bool ServiceLayerFunctionality::valid_parent_id(const std::string& parent_id) {
  // Try to convert the parent ID to an integer, if not possible or the id is
  // greater or equal to the count of the chirps, the id is invalid
  try {
    if ((parent_id != "-1") &&
        ((std::stoi(parent_id) >= std::stoi(chirp_count())) ||
         (std::stoi(parent_id) < 0))) {
      return false;
    }
  } catch (std::invalid_argument&) {
    return false;
  }
  // return true when the id has passed the above tests
  return true;
}

void ServiceLayerFunctionality::create_chirp(Chirp* chirp,
                                             const std::string& username,
                                             const std::string& text,
                                             const std::string& parent_id,
                                             const std::string& chirp_id) {
  // create timestamp for chirp
  int64_t seconds = google::protobuf::util::TimeUtil::TimestampToSeconds(
      google::protobuf::util::TimeUtil::GetCurrentTime());
  int64_t useconds = google::protobuf::util::TimeUtil::TimestampToMicroseconds(
      google::protobuf::util::TimeUtil::GetCurrentTime());

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

void ServiceLayerFunctionality::add_reply(const std::string& chirp_id,
                                          const std::string& parent_id) {
  // Form key of parent chirp id's replies and retrieve replies
  const std::string kReplyParentKey = "reply::" + parent_id;
  std::string replies_serial = client_->get(kReplyParentKey);
  Replies replies;
  replies.ParseFromString(replies_serial);

  // add chirp id to the end of the rest of the replies
  replies.add_id(chirp_id);

  // add replies back to the database
  replies.SerializeToString(&replies_serial);
  client_->put(kReplyParentKey, replies_serial);
  return;
}

std::string ServiceLayerFunctionality::chirp_count() {
  // Retrieve current chirp count
  const std::string kChirpCountKey = "chirp_count::";
  std::string chirp_count_serial = client_->get(kChirpCountKey);
  // If count is an empty string, there have been no chirps added yet
  if (chirp_count_serial.empty()) {
    return "0";
  }
  return chirp_count_serial;
}

void ServiceLayerFunctionality::increment_chirp_count() {
  // Retrieve chirp count from key value store
  const std::string kChirpCountKey = "chirp_count::";
  std::string chirp_count_serial = client_->get(kChirpCountKey);
  int count = 0;
  // convert chirp count to an integer
  if (!chirp_count_serial.empty()) {
    count = std::stoi(chirp_count_serial);
  }
  // increment count and restore in database
  count++;
  chirp_count_serial = std::to_string(count);
  client_->put(kChirpCountKey, chirp_count_serial);
}

void ServiceLayerFunctionality::broadcast_chirp(const std::string& username,
                                                Chirp chirp) {
  // create followers key and retrieve followers
  const std::string kFollowersKey = "followers::" + username;
  std::string followers_serial = client_->get(kFollowersKey);
  Followers followers;
  followers.ParseFromString(followers_serial);

  // loop through each follower
  for (int i = 0; i < followers.username_size(); i++) {
    std::string follower_key = "monitor::" + followers.username(i);
    std::string monitor_serial = client_->get(follower_key);

    // if monitoring is true, add chirp to broadcast list for user
    if (monitor_serial != kFalse_) {
      Chirps chirps;
      chirps.ParseFromString(monitor_serial);
      Chirp* monitor_chirp = chirps.add_chirps();
      *monitor_chirp = chirp;
      chirps.SerializeToString(&monitor_serial);
      client_->put(follower_key, monitor_serial);
    }
  }
  std::vector<std::string> hashtags = chirp_hashtag_check(chirp);
  for (int i = 0; i < hashtags.size(); i++) {
    stream_chirp(chirp, hashtags[i]);
  }
}

std::vector<std::string> ServiceLayerFunctionality::chirp_hashtag_check(
    Chirp chirp) {
  std::vector<std::string> hashtags;
  std::string message = chirp.text();
  size_t pos = message.find("#");
  while (pos != std::string::npos) {
    message = message.substr(pos);
    std::string hashtag = message;
    size_t space = hashtag.find(" ");
    if (space != std::string::npos) {
      hashtag = hashtag.substr(0, space);
    }
    hashtags.push_back(hashtag);
    message = message.substr(1);
    pos = message.find("#");
  }
  return hashtags;
}

void ServiceLayerFunctionality::stream_chirp(Chirp chirp,
                                             const std::string& hashtag) {
  LOG(INFO) << "Someone is trying to stream a chirp with hashtag " << hashtag
            << std::endl;
  const std::string kStreamingKey = kHashtagStream_ + hashtag;
  std::string streaming_serial = client_->get(kStreamingKey);
  Streamers streamers;
  streamers.ParseFromString(streaming_serial);

  for (int i = 0; i < streamers.username_size(); i++) {
    std::string stream_key = kUserStream_ + streamers.username(i);
    std::string stream_serial = client_->get(stream_key);
    Chirps chirps;
    if (stream_serial != kFalse_) {
      chirps.ParseFromString(stream_serial);
    }
    Chirp* stream_chirp = chirps.add_chirps();
    *stream_chirp = chirp;
    chirps.SerializeToString(&stream_serial);
    client_->put(stream_key, stream_serial);
  }
}

void ServiceLayerFunctionality::read_thread(const std::string& chirp_id,
                                            std::vector<Chirp>* chirps) {
  // push back current chirp to vector
  const std::string kChirpKey = "chirp::" + chirp_id;
  std::string chirp_serial = client_->get(kChirpKey);
  // ensure you are not taking an empty chirp
  if (!chirp_serial.empty()) {
    Chirp chirp;
    chirp.ParseFromString(chirp_serial);
    chirps->push_back(chirp);
    // Get all ids of the chirps' replies
    std::string kReplyParentKey = "reply::" + chirp_id;
    std::string replies_serial = client_->get(kReplyParentKey);
    // parse replies and return
    Replies replies;
    replies.ParseFromString(replies_serial);
    for (int i = 0; i < replies.id_size(); i++) {
      read_thread(replies.id(i), chirps);
    }
  }
}

void ServiceLayerFunctionality::clear_monitor(const std::string& username) {
  std::lock_guard<std::mutex> lock(sl_func_mtx_);
  // set monitor back to false once user has quit
  const std::string kMonitorKey = "monitor::" + username;
  client_->put(kMonitorKey, kFalse_);
}
