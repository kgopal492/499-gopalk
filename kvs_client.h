#include "KeyValueStore.grpc.pb.h"

#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

using chirp::PutRequest;
using chirp::PutReply;
using chirp::GetRequest;
using chirp::GetReply;
using chirp::DeleteRequest;
using chirp::DeleteReply;
using chirp::KeyValueStore;

#ifndef CHIRP_KVS_CLIENT_H
#define CHIRP_KVS_CLIENT_H

// provides an interface for the service layer to
// submit requests to the key-value store
class KVS_Client {
 public:
  KVS_Client(std::shared_ptr<Channel> channel);
  // take `key` and `value` to insert into key-value table
  // and return whether insertion was successful
  bool put(const std::string& key, const std::string& value);
  // use `key` to return associated values
  std::string get(const std::string& key);
  // delete key value pair associate with `key` parameter
  bool deletekey(const std::string& key);

 private:
  std::unique_ptr<KeyValueStore::Stub> stub_;
};

#endif
