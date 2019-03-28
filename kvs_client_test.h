// TODO: CONSIDER delete class
#include "KeyValueStore.grpc.pb.h"

#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>

#include "kvs_client_interface.h"
#include "kvs_backend.h"

#ifndef CHIRP_KVS_CLIENT_TEST_H
#define CHIRP_KVS_CLIENT_TEST_H

// client for key value store backend (only used in testing)
class KVS_Client_Test : public KVS_Client_Interface{
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
   KVS_Backend kvs_backend_;
};

#endif // CHIRP_KVS_CLIENT_TEST_H
