#include <string>
#include "kvs_server.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::StatusCode;

using chirp::PutRequest;
using chirp::PutReply;
using chirp::GetRequest;
using chirp::GetReply;
using chirp::DeleteRequest;
using chirp::DeleteReply;
using chirp::KeyValueStore;

// `put` function
// - take `key` and `value` to insert into key-value table
//   and return whether insertion was successful
Status KVS_Server::put(ServerContext* context, const PutRequest* request,
                PutReply* reply){
  // TODO: put the key and value into data structure
  return Status::OK;
}

// get function
// - use `key` to return associated values
Status KVS_Server::get(ServerContext* context, const GetRequest* request,
                GetReply* reply){
  // TODO: get value assoc. with `key` from data structure
  return Status::OK;
}

// delete function
// - delete key value pair associate with `key` parameter
Status KVS_Server::deletekey(ServerContext* context, const DeleteRequest* request,
                DeleteReply* reply){
  // TODO: delete key from key-value store
  return Status::OK;
}
