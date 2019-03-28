#ifndef CHIRP_KVS_CLIENT_TEST_H
#define CHIRP_KVS_CLIENT_TEST_H

#include <string>
#include <thread>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>
#include "kvs_client_interface.h"
#include "kvs_backend.h"
#include "KeyValueStore.grpc.pb.h"

// client for key value store backend (only used in testing)
class KeyValueClientTest : public KeyValueClientInterface{
 public:
   // put `key` and `value` into backend key value store
   bool put(const std::string &key, const std::string &value);
   // get `value` from backend key value store, provided `key`
   std::string get(const std::string &key);
   // delete key value pair associate with `key` parameter in backend
   bool deletekey(const std::string &key);
 private:
   // backend is directly stored as a member variable
   // to avoid gRPC in testing
   KeyValueBackend kvs_backend_;
};
#endif // CHIRP_KVS_CLIENT_TEST_H
