#include "KeyValueStore.grpc.pb.h"

#include <mutex>
#include <string>

#include <grpcpp/grpcpp.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using chirp::PutRequest;
using chirp::PutReply;
using chirp::GetRequest;
using chirp::GetReply;
using chirp::DeleteRequest;
using chirp::DeleteReply;
using chirp::KeyValueStore;

#ifndef CHIRP_KVS_SERVER_H
#define CHIRP_KVS_SERVER_H

 // KVS_Server class supports 3 API calls
 // get(), put(), and delete(), and contains
 // a key-value-store
class KVS_Server final : public KeyValueStore::Service {
 public:
  // put key and value pair in key-value store
  Status put(ServerContext* context, const PutRequest* request,
                  PutReply* reply) override;

  // get value based upon key string
  Status get(ServerContext* context, ServerReaderWriter<GetReply, GetRequest>* stream) override;

  // delete key-value pair given key
  Status deletekey(ServerContext* context, const DeleteRequest* request,
                  DeleteReply* reply) override;

 private:
  // store of key value pairs in KVS_Server
  std::map<std::string, std::string> key_value_pairs_;
  std::mutex mtx_;
};
#endif //CHIRP_KVS_Server_H
