#include "chirpclient.h"

ChirpClient::ChirpClient(std::shared_ptr<Channel> channel)
    : stub_(ServiceLayer::NewStub(channel)) {}

// - registers username of new user with key-value store
// - returns true if username is available
bool registeruser(const std::string& username) {
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

// - logs in user to command line
// - returns true if user exists (and login is valid)
Chirp ChirpClient::chirp(const std::string& username, std::string& text, std::string& parent_id) {
  ChirpRequest request;
  request.set_username(username);
  request.set_text(text);
  request.set_parent_id(parent_id);

  ChirpReply reply;

  ClientContext context;

  Status status = stub_->chirp(&context, request, &reply);

  if (status.ok()) {
    return reply.chirp();
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    Chirp chirp; // TODO: return null/invalid value
    return chirp;
  }
}

// - takes text to be chirped, and sends to key-value store
// - returns true if chirp is successfully
//   registered with key-value store
bool ChirpClient::follow(const std::string& username, const std::string& to_follow) {
  // Send username of client and username of chirp user to follow
  FollowRequest request;
  request.set_username(username);
  request.set_to_follow(to_follow);

  FollowReply reply;

  ClientContext context;

  Status status = stub_->follow(&context, request, &reply);

  if (status.ok()) {
    return true;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return false;
  }
}

// - takes chirpID of beginning of thread
// - returns string of chirp thread
const google::protobuf::RepeatedPtrField<chirp::Chirp> ChirpClient::read(const std::string& chirp_id) {
  ReadRequest request;
  request.set_chirp_id(chirp_id);

  ReadReply reply;

  ClientContext context;

  Status status = stub_->read(&context, request, &reply);

  if (status.ok()) {
    return reply.chirps();
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    const google::protobuf::RepeatedPtrField<chirp::Chirp> chirp; // TODO: return null/invalid value
    return chirp;
  }
}

// - waits for service layer to send chirps of following users
void ChirpClient::monitor(const std::string& username) {
  MonitorRequest request;
  request.set_username(username);

  MonitorReply reply;

  ClientContext context;

  stub_->monitor(&context, request);
}