#include <string>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

#ifndef CHIRP_BACKENDCLIENT_H
#define CHIRP_BACKENDCLIENT_H

// provides an interface for the service layer to
// submit requests to the key-value store
class BackendClient {
 public:
  BackendClient(std::shared_ptr<Channel> channel);
  // take `key` and `value` to insert into key-value table
  // and return whether insertion was successful
  bool put(const int& key, const int& value);
  // use `key` to return associated values
  std::string get(const std::string& key);
  // delete key value pair associate with `key` parameter
  bool deletekey(const std::string& key);
};

#endif
