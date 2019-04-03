#include "sl_server.h"
ServiceLayerServer::ServiceLayerServer() : sl_func_(false) {}

Status ServiceLayerServer::registeruser(ServerContext* context, const RegisterRequest* request,
                RegisterReply* reply){
  // if registeruser fails, the username has already been taken,
  // so return an invalid status
  if (!sl_func_.registeruser(request->username())) {
    return Status(StatusCode::INVALID_ARGUMENT, "Username is taken.");
  }
  return Status::OK;
}

Status ServiceLayerServer::chirp(ServerContext* context, const ChirpRequest* request,
                ChirpReply* reply){
  // determine if the user and parent_id are valid before creating a chirp
  if (!sl_func_.user_exists(request->username())) {
    LOG(ERROR) << "User " << request->username() << " does not exist in database." << std::endl;
    return Status(StatusCode::INVALID_ARGUMENT, "User does not exist.");
  } else if (!sl_func_.valid_parent_id(request->parent_id())) {
    return Status(StatusCode::INVALID_ARGUMENT, "Parent ID not valid");
  }

  // create mutable_chirp with reply object
  Chirp* chirp = reply->mutable_chirp();

  // call function to assign values to chirp
  sl_func_.chirp(chirp, request->username(), request->text(), request->parent_id());
  return Status::OK;
}

Status ServiceLayerServer::follow(ServerContext* context, const FollowRequest* request,
                FollowReply* reply){
  // determine if the `username` and `to_follow` are valid before attempting to follow
  if (!sl_func_.user_exists(request->username())) {
    return Status(StatusCode::INVALID_ARGUMENT, "User does not exist.");
  } else if (!sl_func_.user_exists(request->to_follow())) {
    return Status(StatusCode::INVALID_ARGUMENT, "User to follow does not exist.");
  }
  // follow if both parties exist in database
  sl_func_.follow(request->username(), request->to_follow());
  return Status::OK;
}

Status ServiceLayerServer::read(ServerContext* context, const ReadRequest* request,
                ReadReply* reply){
  // retrieve thread starting from given id
  std::vector<Chirp> chirps = sl_func_.read(request->chirp_id());
  // empty vector indicates that the original ID is invalid
  if (!chirps.size()) {
    return Status(StatusCode::INVALID_ARGUMENT, "Invalid chirp ID");
  }
  // Loop through all chirps and make them the reply
  for(int i = 0; i < chirps.size(); i++) {
    Chirp *chirp = reply->mutable_chirps(i);
    *chirp = chirps[i];
  }
  return Status::OK;
}

Status ServiceLayerServer::monitor(ServerContext* context, const MonitorRequest* request, ServerWriter<MonitorReply>* writer){
  // keep polling the database while monitor is true
  while(!context->IsCancelled()) {
    // request the new chirps
    Chirps chirps = sl_func_.monitor(request->username());
    // loop through the chirps and add them to the reply
    for (int i = 0; i < chirps.chirps_size(); i++) {
      // add each chirp to stream
      MonitorReply monitor_reply;
      Chirp *chirp = monitor_reply.mutable_chirp();
      *chirp = chirps.chirps(i);
      writer->Write(monitor_reply);
    }
  }
  // clear the monitor to false that that new requests aren't storec
  sl_func_.clear_monitor(request->username());
  return Status::OK;
}
