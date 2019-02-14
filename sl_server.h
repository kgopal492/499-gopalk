#include "Backend.grpc.pb.h"
#include "Backend.pb.h"
#include "KeyValueStore.grpc.pb.h"
#include "KeyValueStore.pb.h"
#include "ServiceLayer.grpc.pb.h"
#include "ServiceLayer.pb.h"
#include "kvs_client.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
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

using chirp::Users;
using chirp::Chirps;
using chirp::Replies;
using chirp::Reply;
using chirp::Follow;
using chirp::Following;
using chirp::Followers;
#ifndef CHIRP_SL_SERVER_H
#define CHIRP_SL_SERVER_H

// implementation of service layer
// takes request from command line clients
class SL_Server final : public ServiceLayer::Service {
 public:
  // constructor, initializes KVS_Client
  SL_Server();
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
                  ServerWriter<MonitorReply>* writer) override;
 //TODO: serialize and move data to Key Value Store
 private:
  // client for keyvaluestore layer
  KVS_Client client_;
  // mutex to lock sl for monitor and other functions
  std::mutex mtx_;
};

#endif
