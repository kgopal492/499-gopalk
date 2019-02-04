#include "servicelayerimpl.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using chirp::Chirp;
using chirp::RegisterRequest;
using chirp::RegisterReply;
using chirp::ChirpRequest;
using chirp::ChirpReply;
using chirp::FollowRequest;
using chirp::FollowReply;
using chirp::ReadRequest;
using chirp::ReadReply;
using chirp::MonitorRequest;
using chirp::MonitorReply;

// register user with backend service
Status ServiceLayerImpl::registeruser(ServerContext* context, const RegisterRequest* request,
                RegisterReply* reply){
  // TODO: register with backend
  if(users.find(request.username()) == users.end()) {
    users.insert(username);
  }
  else {
    std::cout << "User is already in the database" << std::endl;
  }
  return Status::OK;
}

// allow user to send chirp and register with backend
Status ServiceLayerImpl::chirp(ServerContext* context, const ChirpRequest* request,
                ChirpReply* reply){
  // TODO: insert chirp into backend service
  return Status::OK;
}

// allow user to follow another user (store in backend)
Status ServiceLayerImpl::follow(ServerContext* context, const FollowRequest* request,
                FollowReply* reply){
  //TODO: allow chirp to follow another user by calling backend service
  return Status::OK;
}

// allow user to read a thread
Status ServiceLayerImpl::read(ServerContext* context, const ReadRequest* request,
                ReadReply* reply){
  //TODO: get thread from backend service and return
  return Status::OK;
}

// allow user to monitor followers
Status ServiceLayerImpl::monitor(ServerContext* context, const MonitorRequest* request,
                MonitorReply* reply){
 //TODO: process user's following list and broadcast chirps
  return Status::OK;
}
