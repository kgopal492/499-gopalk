#include "ServiceLayer.grpc.pb.h"
#include "ServiceLayer.pb.h"

#include <grpcpp/grpcpp.h>

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
using chirp::ServiceLayer;

#ifndef CHIRP_SERVICELAYERIMPL_H
#define CHIRP_SERVICELAYERIMPL_H

// implementation of service layer
// takes request from command line clients
class ServiceLayerImpl final : public ServiceLayer::Service {
 public:
  // register user with backend service
  Status registeruser(ServerContext* context, const RegisterRequest* request,
                  RegisterReply* reply) override;
  // allow user to send chirp and register with backend
  Status chirp(ServerContext* context, const ChirpRequest* request,
                  ChirpReply* reply) override;
  // allow user to follow another user (store in backend)
  Status follow(ServerContext* context, const FollowRequest* request,
                  FollowReply* reply) override;
  // allow user to read a thread
  Status read(ServerContext* context, const ReadRequest* request,
                  ReadReply* reply) override;
  // allow user to monitor followers
  Status monitor(ServerContext* context, const MonitorRequest* request,
                  MonitorReply* reply);
};

#endif
