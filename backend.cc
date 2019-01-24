#include <string>
#include "backend.h"

// `put` function
// - take `key` and `value` to insert into key-value table
//   and return whether insertion was successful
bool Backend::put(const string &key, const string &value) {
  // TODO: put the key and value into data structure
  return true;
}

// get function
// - use `key` to return associated values
string Backend::get(const string &key) {
  // TODO: get value assoc. with `key` from data structure
  return "";
}

// delete function
// - delete key value pair associate with `key` parameter
bool Backend::delete(const string &key) {
  // TODO: delete key from key-value store
  return true;
}