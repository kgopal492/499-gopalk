#include <string>
#include <thread>
#include <iostream>
#include <memory>
#include <stack>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>
#include "KeyValueStore.grpc.pb.h"
#include "kvs_client_interface.h"

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

// allows service layer to submit requests to
// the key-value store server over gRPC
class KeyValueClient : public KeyValueClientInterface {
 public:
   // construct client with channel to KeyValueServer class
   KeyValueClient(std::shared_ptr<Channel> channel);
   // take `key` and `value` to insert into key-value table
   // and return whether insertion was successful
   bool put(const std::string &key, const std::string &value);
   // use `key` to return associated values
   std::string get(const std::string &key);
   // delete key value pair associate with `key` parameter
   bool deletekey(const std::string &key);
 private:
   //`stub_` to send and receive data over gRPC
   std::unique_ptr<KeyValueStore::Stub> stub_;
};
#endif // CHIRP_KVS_CLIENT_H
