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
// take `key` and `value` to insert into key-value table
// and return whether insertion was successful
Status KVS_Server::put(ServerContext* context, const PutRequest* request,
                PutReply* reply){
  // create iterator for `key_value_pairs` map
  std::map<std::string, std::string>::iterator key_it = key_value_pairs_.find(request->key());
  
  // if key exists in database, update value, else, insert new key value pair
  if(key_it != key_value_pairs_.end()) {
    key_value_pairs_[request->key()] = request->value();
  }
  else {
    key_value_pairs_.insert({request->key(), request->value()});
  }
  return Status::OK;
}

// get function
// use `key` to return associated values
// TODO: change from multiple requests and replies to just one
Status KVS_Server::get(ServerContext* context, ServerReaderWriter<GetReply, GetRequest>* stream) {
  
  // Read all GetRequests from stream
  GetRequest request;
  while (stream->Read(&request)) {
    
    // find key in `key_value_pairs`, if it exists add to reply, else return error status
    std::map<std::string, std::string>::iterator key_it = key_value_pairs_.find(request.key());
    if(key_it != key_value_pairs_.end()) {
      GetReply reply;
      reply.set_value(key_value_pairs_[request.key()]);
      stream->Write(reply);
    }
    else {
      return Status(StatusCode::INVALID_ARGUMENT, "key: " + request.key() + " not in key value store");
    }
  }

  return Status::OK;
}

// delete function
// delete key value pair associate with `key` parameter
Status KVS_Server::deletekey(ServerContext* context, const DeleteRequest* request,
                DeleteReply* reply){
  std::map<std::string, std::string>::iterator key_it = key_value_pairs_.find(request->key());
  if(key_it != key_value_pairs_.end()) {
    return Status::OK;
  }
  else {
    return Status(StatusCode::INVALID_ARGUMENT, "key not in key value store");
  }
}
