#include "kvs_client.h"

KVS_Client::KVS_Client(std::shared_ptr<Channel> channel)
    : stub_(KeyValueStore::NewStub(channel)) {}

// take `key` and `value` to insert into key-value table
// and return whether insertion was successful
bool KVS_Client::put(const std::string &key, const std::string &value) {
  // Send key and value to key-value store
  PutRequest request;
  request.set_key(key);
  request.set_value(value);

  // Store response in reply
  PutReply reply;

  // Client's context for additional information to server, or rpc options
  ClientContext context;

  // Send the rpc
  Status status = stub_->put(&context, request, &reply);

  // Determine if the status is ok, then process
  if (status.ok()) {
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}

// use `key` to return associated values
// TODO: change from stream to single request and reply
std::string KVS_Client::get(const std::string& key) {
  ClientContext context;

  std::shared_ptr<ClientReaderWriter<GetRequest, GetReply> > stream(
      stub_->get(&context));
  GetRequest getRequest;
  getRequest.set_key(key);
  std::vector<GetRequest> getKeys{getRequest};
  for (const GetRequest& getKey : getKeys) {
    stream->Write(getKey);
  }
  stream->WritesDone();

  GetReply getReply;
  while (stream->Read(&getReply)) {
    return getReply.value();
  }
  Status status = stream->Finish();
  if (!status.ok()) {
    std::cout << "Get request failed." << std::endl;
  }
  return "";
}

// delete key value pair associate with `key` parameter
bool KVS_Client::deletekey(const std::string& key) {
  DeleteRequest request;
  request.set_key(key);

  DeleteReply reply;

  ClientContext context;

  Status status = stub_->deletekey(&context, request, &reply);

  if (status.ok()) {
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}
