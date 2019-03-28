// TODO: CONSIDER delete class
#include "kvs_client_test.h"

bool KVS_Client_Test::put(const std::string &key, const std::string &value) {
  // return result of put request on key value backend class
  // true result indicates put operation successful
  // false indicates `key` or `value` is an empty string
  return kvs_backend_.put(key, value);
}

std::string KVS_Client_Test::get(const std::string& key) {
  // return value of get request on key value backend
  return kvs_backend_.get(key);
}

bool KVS_Client_Test::deletekey(const std::string& key) {
  // return result of delete request on key value backend
  // successful delete returns true
  // if key does not exist in map, returns false
  return kvs_backend_.deletekey(key);
}
