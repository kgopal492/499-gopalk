#include "sl_server.h"
// TODO: ensure that no empty key is placed in database
SL_Server::SL_Server() : sl_func_(false) {}

Status SL_Server::registeruser(ServerContext* context, const RegisterRequest* request,
                RegisterReply* reply){
  if (!sl_func_.registeruser(request->username())) {
    return Status(StatusCode::INVALID_ARGUMENT, "Username is taken.");
  }
  return Status::OK;
}

Status SL_Server::chirp(ServerContext* context, const ChirpRequest* request,
                ChirpReply* reply){
  if (!sl_func_.user_exists(request->username())) {
    return Status(StatusCode::INVALID_ARGUMENT, "User does not exist.");
  } else if (!sl_func_.valid_parent_id(request->parent_id())) {
    return Status(StatusCode::INVALID_ARGUMENT, "Parent ID not valid");
  }
  Chirp* chirp = sl_func_.chirp(request->username(), request->text(), request->parent_id());
  reply->set_allocated_chirp(chirp);
  return Status::OK;
}

Status SL_Server::follow(ServerContext* context, const FollowRequest* request,
                FollowReply* reply){
  int followResult = sl_func_.follow(request->username(), request->to_follow());
  if (followResult == 1) {
    return Status(StatusCode::INVALID_ARGUMENT, "User does not exist.");
  } else if (followResult == 2) {
    return Status(StatusCode::INVALID_ARGUMENT, "User to follow does not exist.");
  }
  return Status::OK;
}

Status SL_Server::read(ServerContext* context, const ReadRequest* request,
                ReadReply* reply){
  std::vector<Chirp> chirps = sl_func_.read(request->chirp_id());
  if(!chirps.size()) {
    return Status(StatusCode::INVALID_ARGUMENT, "Invalid chirp ID");
  }
  for(int i = 0; i < chirps.size(); i++) {
    Chirp *chirp = reply->add_chirps();
    *chirp = chirps[i];
  }
  return Status::OK;
}

Status SL_Server::monitor(ServerContext* context, const MonitorRequest* request, ServerWriter<MonitorReply>* writer){
  while(!context->IsCancelled()) {
    Chirps chirps = sl_func_.monitor(request->username());
    for (int i = 0; i < chirps.chirps_size(); i++) {
      // add each chirp to stream
      MonitorReply monitor_reply;
      Chirp *chirp = new Chirp();
      *chirp = chirps.chirps(i);
      monitor_reply.set_allocated_chirp(chirp);
      writer->Write(monitor_reply);
    }
  }
  sl_func_.clear_monitor(request->username());
  return Status::OK;
}
