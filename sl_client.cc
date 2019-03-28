#include "sl_client.h"

ServiceLayerClient::ServiceLayerClient(std::shared_ptr<Channel> channel)
    : stub_(ServiceLayer::NewStub(channel)) {}

bool ServiceLayerClient::registeruser(const std::string& username) {
  // Send username to the service layer
  RegisterRequest request;
  request.set_username(username);

  // Store response in reply
  RegisterReply reply;

  // Client's context for additional information to server, or rpc options
  ClientContext context;

  // Send the rpc
  Status status = stub_->registeruser(&context, request, &reply);

  // Determine if the status is ok, then process
  if (status.ok()) {
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}

Chirp ServiceLayerClient::chirp(const std::string& username, const std::string& text, const std::string& parent_id) {
  // create request with username, text, and parent_id
  ChirpRequest request;
  request.set_username(username);
  request.set_text(text);
  request.set_parent_id(parent_id);

  // create object to hold response
  ChirpReply reply;

  // declare context for additional information
  ClientContext context;

  // send the rpc
  Status status = stub_->chirp(&context, request, &reply);

  // if status ok, print success message and return chirp
  // else, print error message and return empty chirp
  if (status.ok()) {
    std::cout << "status ok" << std::endl;
    return reply.chirp();
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    Chirp chirp;
    return chirp;
  }
}

bool ServiceLayerClient::follow(const std::string& username, const std::string& to_follow) {
  // Send username of client and username of chirp user to follow
  FollowRequest request;
  request.set_username(username);
  request.set_to_follow(to_follow);

  // create reply object
  FollowReply reply;

  // create context to hold additional information from server
  ClientContext context;

  // send rpc
  Status status = stub_->follow(&context, request, &reply);

  // if status ok, return true
  // else, print error message and return false
  if (status.ok()) {
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}

const google::protobuf::RepeatedPtrField<chirp::Chirp> ServiceLayerClient::read(const std::string& chirp_id) {
  // create request with id of chirp to read
  ReadRequest request;
  request.set_chirp_id(chirp_id);

  // create object to store reply
  ReadReply reply;

  // create context for additional information from server
  ClientContext context;

  // send rpc
  Status status = stub_->read(&context, request, &reply);

  // if status ok, return chirps
  // else, print error and return empty chirps
  if (status.ok()) {
    return reply.chirps();
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    const google::protobuf::RepeatedPtrField<chirp::Chirp> chirp; // TODO: return null/invalid value
    return chirp;
  }
}

void ServiceLayerClient::monitor(const std::string& username) {
  // create request with username of monitoring user
  MonitorRequest request;
  request.set_username(username);

  // create object to store stream of replies
  MonitorReply reply;

  // create context for additional information from server
  ClientContext context;

  // create `reader` to read stream of data from server
  std::unique_ptr<ClientReader<MonitorReply> > reader(stub_->monitor(&context, request));
  while(reader->Read(&reply)) {
    std::cout << "\"" << reply.chirp().text() << "\"" << " - " << reply.chirp().username() << " ID: " << reply.chirp().id() << std::endl;
  }
  Status status = reader->Finish();
  if (!status.ok()) {
    std::cout << "Monitor rpc failed" << std::endl;
  }
}
