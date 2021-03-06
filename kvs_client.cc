#include "kvs_client.h"

KeyValueClient::KeyValueClient(std::shared_ptr<Channel> channel)
    : stub_(KeyValueStore::NewStub(channel)) {}

bool KeyValueClient::put(const std::string &key, const std::string &value) {
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
    LOG(INFO) << "Put return status ok form KeyValueClient." << std::endl;
    return true;
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message() << std::endl;
    return false;
  }
}

std::string KeyValueClient::get(const std::string& key) {
  ClientContext context;
  // create stream object to write and read keys and values from
  std::shared_ptr<ClientReaderWriter<GetRequest, GetReply> > stream(
      stub_->get(&context));

  // create GetRequest for given `key` and pass to kvs_server
  GetRequest getRequest;
  getRequest.set_key(key);
  std::vector<GetRequest> getKeys{getRequest};
  for (const GetRequest& getKey : getKeys) {
    stream->Write(getKey);
  }
  stream->WritesDone();

  // read GetReplies from server
  GetReply getReply;

  // return first GetReply object since
  // this implementation assumes only one reply
  // that contains all the necessary information
  while (stream->Read(&getReply)) {
    LOG(INFO) << "Successful get request."<< std::endl;
    return getReply.value();
  }
  Status status = stream->Finish();
  LOG(ERROR) << "No return value provided from get request."<< std::endl;
  return "";
}

bool KeyValueClient::deletekey(const std::string& key) {
  // create request object
  DeleteRequest request;
  request.set_key(key);

  // create object to hold reply
  DeleteReply reply;
  ClientContext context;

  // send to server
  Status status = stub_->deletekey(&context, request, &reply);

  // Determine if status is ok, and log an appropriate message and return
  // from function
  if (status.ok()) {
    LOG(INFO) << "Delete request status ok from KeyValueClient" << std::endl;
    return true;
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}
