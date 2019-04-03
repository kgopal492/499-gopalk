#ifndef CHIRP_KVS_SERVER_H
#define CHIRP_KVS_SERVER_H

#include <mutex>
#include <string>
#include <iostream>
#include <memory>
#include <stack>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>
#include "KeyValueStore.grpc.pb.h"
#include "kvs_backend.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using grpc::StatusCode;
using chirp::PutRequest;
using chirp::PutReply;
using chirp::GetRequest;
using chirp::GetReply;
using chirp::DeleteRequest;
using chirp::DeleteReply;
using chirp::KeyValueStore;

// KeyValueServer class supports takes request from
// KeyValueClient in Service Layer to support
// get, put, and delete requests on key value backend
class KeyValueServer final : public KeyValueStore::Service {
 public:
   // put key and value pair in `key_value_pairs_` data structure
   Status put(ServerContext *context, const PutRequest *request,
              PutReply *reply) override;
   // get value based upon key string from `key_value_pairs_` member variable
   Status get(ServerContext *context,
              ServerReaderWriter<GetReply, GetRequest> *stream) override;
   // delete key-value pair given key from `key_value_pairs_` variable
   Status deletekey(ServerContext *context, const DeleteRequest *request,
                    DeleteReply *reply) override;
 private:
  // class that stores key value pairs as a map
  KeyValueBackend kvs_backend_;
};
#endif // CHIRP_KVS_SERVER_H
