#ifndef CHIRP_SERVICE_H
#define CHIRP_SERVICE_H

#include <string>

/*
Service class supports requests from clients
via their command lines and call key-value store
to process requests
*/
class Service {
 public:
  // register user with backend data store
  bool registerUser(const string &username);

  // log user in to command line
  bool loginUser(const string &username);

  // broadcast chirp from user to data store
  bool chirp(const string &text);

  // reply to chirpID (with given data store)
  bool reply(const string &chirpID);

  // add a user to following list
  bool follow(const string &username);

  // read thread given starting chirpID
  string read(const string &chirpID);

  // view stream of chirps from followers
  bool monitor();

 private:
  // TODO: include followers
};
#endif //CHIRP_SERVICE_H