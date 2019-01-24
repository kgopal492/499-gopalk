#ifndef CHIRP_BACKEND_H
#define CHIRP_BACKEND_H

#include <string>

class Backend {
  public:
    // put key and value pair in key-value store
    bool put(const string &key, const string &value);

    // get value from key string
    string get(const string &key);

    // delete key-value pair given key
    bool delete(const string &key);

  private:
    // store of key value pairs in backend
    std::map<string, string> key_value_pairs_;
};
#endif //CHIRP_BACKEND_H