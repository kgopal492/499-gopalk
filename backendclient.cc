#include "backendclient.h"

class BackendClient {
 public:
  BackendClient(std::shared_ptr<Channel> channel)
      : stub_(KeyValueStore::NewStub(channel)) {}

  // take `key` and `value` to insert into key-value table
  // and return whether insertion was successful
  bool put(const int& key, const int& value) {
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
  std::string get(const std::string& key) {
    GetRequest request;
    request.set_key(key);

    GetReply reply;

    ClientContext context;

    Status status = stub_->get(&context, request, &reply);

    if (status.ok()) {
      return reply.value();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "";
    }
  }

  // delete key value pair associate with `key` parameter
  bool deletekey(const std::string& key) {
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
};