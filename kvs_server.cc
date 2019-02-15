#include "kvs_server.h"

Status KVS_Server::put(ServerContext* context, const PutRequest* request,
                PutReply* reply){
  mtx_.lock();
  // whether key exists in database or not, update value
  key_value_pairs_[request->key()] = request->value();
  mtx_.unlock();
  return Status::OK;
}

Status KVS_Server::get(ServerContext* context, ServerReaderWriter<GetReply, GetRequest>* stream) {
  mtx_.lock();
  // Read all GetRequests from stream
  GetRequest request;
  while (stream->Read(&request)) {

    // find key in `key_value_pairs`, if it exists add to reply, else return
    // error status implementation assumes only one value to return
    std::map<std::string, std::string>::iterator key_it = key_value_pairs_.find(request.key());
    if(key_it != key_value_pairs_.end()) {
      GetReply reply;
      reply.set_value(key_value_pairs_[request.key()]);
      stream->Write(reply);
    } else {
      mtx_.unlock();
      return Status(StatusCode::INVALID_ARGUMENT, "key: " + request.key() + " not in key value store");
    }
  }
  mtx_.unlock();
  return Status::OK;
}

Status KVS_Server::deletekey(ServerContext* context, const DeleteRequest* request,
                DeleteReply* reply){
  mtx_.lock();

  // determine if key exists in database to return error
  std::map<std::string, std::string>::iterator key_it = key_value_pairs_.find(request->key());
  if(key_it != key_value_pairs_.end()) {
    key_value_pairs_.erase(request->key());
    mtx_.unlock();
    return Status::OK;
  } else {
    mtx_.unlock();
    return Status(StatusCode::INVALID_ARGUMENT, "key not in key value store");
  }
}
