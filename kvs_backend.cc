#include "kvs_backend.h"

bool KeyValueBackend::put(const std::string &key, const std::string &value) {
  // if key or value is an empty string, return false
  if (key.empty() || value.empty()) {
    return false;
  }
  // lock the `key_value_pairs_` structure before inserting or updating pair
  std::lock_guard<std::mutex> lock(key_value_mtx_);
  // whether key exists in database or not, update value
  // since any put request can overwrite the previous value
  key_value_pairs_[key] = value;
  // return true once key is inserted
  return true;
}

std::string KeyValueBackend::get(const std::string &key) {
  // lock the `key_value_pairs_` structure before getting value
  // (to ensure structure doesn't change if thread is preempted)
  std::lock_guard<std::mutex> lock(key_value_mtx_);
  // create an iterator to find key in unordered_map
  std::unordered_map<std::string, std::string>::iterator key_it = key_value_pairs_.find(key);
  // return key if it exists, else return empty string
  if (key_it != key_value_pairs_.end()) {
    return key_value_pairs_[key];
  }
  return "";
}

bool KeyValueBackend::deletekey(const std::string &key) {
  // lock the `key_value_pairs_` structure before deleting value
  // in case another thread attempts to preempt and access value
  std::lock_guard<std::mutex> lock(key_value_mtx_);
  // determine if key exists in database to return error
  std::unordered_map<std::string, std::string>::iterator key_it = key_value_pairs_.find(key);
  // return true if key sucessfully deleted from database
  if (key_it != key_value_pairs_.end()) {
    key_value_pairs_.erase(key);
    return true;
  }
  // if key does not exists in database, return false
  return false;
}
