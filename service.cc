#include "service.h"

// registerUser function
// - registers username of new user with key-value store
// - returns true if username is available
bool Service::registerUser(string username) {
  // TODO: register user with key-value store
  return true;
}

// loginUser function
// - logs in user to command line
// - returns true if user exists (and login is valid)
bool Service::loginUser(string username) {
  // TODO: login user in command line
  return true;
}

// chirp function
// - takes text to be chirped, and sends to key-value store
// - returns true if chirp is successfully
//   registered with key-value store
bool Service::chirp(string text) {
  // TODO: send chirp to key-value store
  return true;
}

// reply function
// - takes chirpID of user to reply to create reply chirp
// - returns true if reply is successful
bool Service::reply(string chirpID) {
  // TODO: reply to chirpID, register response with key-value store
  return true;
}

// follow function
// - takes username of chirp user to follow, and enters follow in
//   key-value store
// - returns true if user exists and is possible to follow
bool Service::follow(string username) {
  // TODO: follow user, put info in key-value store
  return true;
}

// read function
// - takes chirpID of beginning of thread
// - returns string of chirp thread
string Service::read(string chirpID) {
  // TODO: read chirp thread from key-value store
  return "";
}

// monitor function
// - waits for service layer to send chirps of following users
void Service::monitor() {
  // TODO: receive chirps from following users
}