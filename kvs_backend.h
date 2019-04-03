#ifndef CHIRP_KVS_BACKEND_H
#define CHIRP_KVS_BACKEND_H

#include <mutex>
#include <string>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <stack>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>
#include "KeyValueStore.grpc.pb.h"

// stores key value store as an unordered_map data structure
// after communicating with KeyValueServer
class KeyValueBackend {
 public:
   // take `key` and `value` to insert into key-value map
   // and return whether insertion was successful
   bool put(const std::string &key, const std::string &value);
   // use `key` to return associated string value from map
   std::string get(const std::string &key);
   // delete key value pair associated with `key` parameter
   bool deletekey(const std::string &key);
 private:
  // store of key value pairs in KeyValueServer
  std::unordered_map<std::string, std::string> key_value_pairs_;
  // use mutex to lock functions while key_value_pairs_ is being modified
  std::mutex key_value_mtx_;
};
#endif // CHIRP_KVS_BACKEND_H
