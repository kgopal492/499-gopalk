#include "service.h"

/*
registerUser function
- input: takes username of new user
- output: returns true if username is available
- registers user with key-value store
*/
bool Service::registerUser(string username) {

}

/*
loginUser function
- input: takes username of user
- output: returns true if user exists (and login is valid)
- logs in user to command line
*/
bool Service::loginUser(string username) {

}

/*
chirp function
- input: takes text to be chirped
- output: returns true if chirp is successfull 
  registered with key-value store
- creates a chirp and sends chirp to key value store
*/
bool Service::chirp(string text) {

}

/*
reply function
- input: takes chirpID of user to reply to
- output: returns true if reply is successful
- creates a chirp in response to another chirpID
*/
bool Service::reply(string chirpID) {

}

/*
follow function
- input: takes username of chirp user to follow
- output: returns true if user exists and is possible to follow
- allows user to follow other user
*/
bool Service::follow(string username) {

}

/*
read function
- input: takes chirpID of beginning of thread
- output: returns string of chirp thread
- takes beginning chirp thread ID and returns
  all responses
*/
string Service::read(string chirpID) {

}

/*
monitor function
- input: none
- output: none
- waits for service layer to send chirps of following users
*/
void Service::monitor() {

}