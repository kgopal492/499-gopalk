#ifndef CHIRP_BACKEND_H
#define CHIRP_BACKEND_H

#include <string>

 // Backend class supports 3 API calls
 // get(), put(), and delete(), and contains
 // a key-value-store
class Backend {
 public:
  // put key and value pair in key-value store
  bool put(const std::string &key, const std::string &value);

  // get value based upon key string
  string get(const std::string &key);

  // delete key-value pair given key
  bool delete(const std::string &key);

 private:
  // store of key value pairs in backend
  std::map<std::string, std::string> key_value_pairs_;
};
#endif //CHIRP_BACKEND_H
