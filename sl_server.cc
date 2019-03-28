#include "sl_server.h"
// TODO: ensure that no empty key is placed in database
ServiceLayerServer::ServiceLayerServer() : sl_func_(false) {}

Status ServiceLayerServer::registeruser(ServerContext* context, const RegisterRequest* request,
                RegisterReply* reply){
  if (!sl_func_.registeruser(request->username())) {
    return Status(StatusCode::INVALID_ARGUMENT, "Username is taken.");
  }
  return Status::OK;
}

Status ServiceLayerServer::chirp(ServerContext* context, const ChirpRequest* request,
                ChirpReply* reply){
  if (!sl_func_.user_exists(request->username())) {
    return Status(StatusCode::INVALID_ARGUMENT, "User does not exist.");
  } else if (!sl_func_.valid_parent_id(request->parent_id())) {
    return Status(StatusCode::INVALID_ARGUMENT, "Parent ID not valid");
  }
  Chirp* chirp = reply->mutable_chirp();
  sl_func_.chirp(chirp, request->username(), request->text(), request->parent_id());
  return Status::OK;
}

Status ServiceLayerServer::follow(ServerContext* context, const FollowRequest* request,
                FollowReply* reply){
  int followResult = sl_func_.follow(request->username(), request->to_follow());
  if (followResult == 1) {
    return Status(StatusCode::INVALID_ARGUMENT, "User does not exist.");
  } else if (followResult == 2) {
    return Status(StatusCode::INVALID_ARGUMENT, "User to follow does not exist.");
  }
  return Status::OK;
}

Status ServiceLayerServer::read(ServerContext* context, const ReadRequest* request,
                ReadReply* reply){
  std::vector<Chirp> chirps = sl_func_.read(request->chirp_id());
  if (!chirps.size()) {
    return Status(StatusCode::INVALID_ARGUMENT, "Invalid chirp ID");
  }
  for(int i = 0; i < chirps.size(); i++) {
    Chirp *chirp = reply->add_chirps();
    *chirp = chirps[i];
  }
  return Status::OK;
}

Status ServiceLayerServer::monitor(ServerContext* context, const MonitorRequest* request, ServerWriter<MonitorReply>* writer){
  while(!context->IsCancelled()) {
    Chirps chirps = sl_func_.monitor(request->username());
    for (int i = 0; i < chirps.chirps_size(); i++) {
      // add each chirp to stream
      MonitorReply monitor_reply;
      Chirp *chirp = monitor_reply.mutable_chirp();
      *chirp = chirps.chirps(i);
      writer->Write(monitor_reply);
    }
  }
  sl_func_.clear_monitor(request->username());
  return Status::OK;
}
