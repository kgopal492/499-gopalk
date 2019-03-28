#include "kvs_server.h"

Status KeyValueServer::put(ServerContext* context, const PutRequest* request,
                PutReply* reply){
  // Attempt to put key and value into key value backend map
  // If put request returns true, operation is successful
  if (kvs_backend_.put(request->key(), request->value())) {
    //LOG(INFO) << "Put request succeeded." << std::endl;
    return Status::OK;
  }
  // Return false when put request returns false, indicating key or value is empty
  LOG(ERROR) << "Put request failed because key or value is empty string." << std::endl;
  return Status(StatusCode::INVALID_ARGUMENT, "Key or value is an empty string.");
}

Status KeyValueServer::get(ServerContext* context, ServerReaderWriter<GetReply, GetRequest>* stream) {
  // Read all GetRequests from stream
  GetRequest request;
  while (stream->Read(&request)) {
    // get value from key value backend of current key in stream
    std::string value = kvs_backend_.get(request.key());
    // if value is an empty string, the key is not present in key value map
    if (value.empty()) {
      LOG(ERROR) << "Get request failed since key not in key value store." << std::endl;
      return Status(StatusCode::INVALID_ARGUMENT, "Key: " + request.key() + " not in key value store.");
    }
    // if key exists in key value pair map,  write to output stream
    GetReply reply;
    reply.set_value(value);
    stream->Write(reply);
  }
  // Return successful status if each key was present in database
  LOG(INFO) << "Get request succeeded." << std::endl;
  return Status::OK;
}

Status KeyValueServer::deletekey(ServerContext* context, const DeleteRequest* request,
                DeleteReply* reply){
  // Call deletekey request on kvs backend class for given key requested
  if (kvs_backend_.deletekey(request->key())) {
    LOG(INFO) << "Delete request succeeded." << std::endl;
    return Status::OK;
  }
  // if deletekey request on kvs backend returns false, key is not present in database
  LOG(ERROR) << "Key not present in key value store - delete failed." << std::endl;
  return Status(StatusCode::INVALID_ARGUMENT, "key not in key value store");
}
