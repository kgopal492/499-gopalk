#ifndef CHIRP_KVS_CLIENT_INTERFACE_H
#define CHIRP_KVS_CLIENT_INTERFACE_H

#include <string>

// provides an interface for KVS_Client and KVS_Client_Test
// classes to inherit from (to differentiate between key value
// store clients used in testing and production)
class KeyValueClientInterface {
 public:
   // take `key` and `value` to insert into key-value table
   // and return whether insertion was successful
   virtual bool put(const std::string &key, const std::string &value) = 0;
   // use `key` to return associated values
   virtual std::string get(const std::string &key) = 0;
   // delete key value pair associate with `key` parameter
   virtual bool deletekey(const std::string &key) = 0;
};
#endif // CHIRP_KVS_INTERFACE_H
