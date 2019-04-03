#include <gtest/gtest.h>
#include "kvs_backend.h"

// Test that putting null key and null value returns false
TEST(KeyValueBackendTest, EmptyPut) {
  KeyValueBackend backend;
  //If value is empty, put should return false
  EXPECT_EQ(backend.put("key", ""), false);
  //If key is empty, put should return false
  EXPECT_EQ(backend.put("", "value"), false);
  //If key and value is empty, put should return false
  EXPECT_EQ(backend.put("", ""), false);
}

// Test that with a non-empty key and non-empty value put returns true
TEST(KeyValueBackendTest, RegularPut) {
  KeyValueBackend backend;
  // regular size key and value strings
  EXPECT_EQ(backend.put("key", "value"), true);
  // longer words should be able to be inserted into key value store
  EXPECT_EQ(backend.put("keylmnopqrstuvwxyz", "valueabcdefghijklmnop"), true);
  // one character strings can be put
  EXPECT_EQ(backend.put("k", "v"), true);
  // numerical characters can be put
  EXPECT_EQ(backend.put("1", "9"), true);
  // non alpha-numeric characters can be put
  EXPECT_EQ(backend.put("$", "]"), true);
}

// Test that a valid put request will return value when get is called
TEST(KeyValueBackendTest, GetReturnsPut) {
  KeyValueBackend backend;
  // short strings
  backend.put("key", "value");
  EXPECT_EQ(backend.get("key"), "value");
  // medium length strings
  backend.put("smkeylmnopqrstuvwxyz", "lkjkeylmnopqrstuvwxyz");
  EXPECT_EQ(backend.get("smkeylmnopqrstuvwxyz"), "lkjkeylmnopqrstuvwxyz");
  // one character strings
  backend.put("a", "d");
  EXPECT_EQ(backend.get("a"), "d");
  // numerical strings
  backend.put("12984", "9928");
  EXPECT_EQ(backend.get("12984"), "9928");
  // non-aphanumeric strings
  backend.put("$(#&#&)", "]^$%@^");
  EXPECT_EQ(backend.get("$(#&#&)"), "]^$%@^");
}

// Get a value that is not in that database, should return ""
TEST(KeyValueBackendTest, GetNonExistantKey) {
  KeyValueBackend backend;
  const std::string kEmptyString = "";
  EXPECT_EQ(backend.get("abcdef"), kEmptyString);
  EXPECT_EQ(backend.get("(@&#^)"), kEmptyString);
  EXPECT_EQ(backend.get("394727"), kEmptyString);
  EXPECT_EQ(backend.get("kshJH8&@&^"), kEmptyString);
}

// Deleting existing key should return true
TEST(KeyValueBackendTest, DeleteExistingKey) {
  KeyValueBackend backend;
  // put all keys
  backend.put("key", "value");
  backend.put("KASH", "MONEY");
  backend.put("EAT", "food");
  backend.put("y", "n");
  backend.put("12", "14");

  // delete all keys (must return true)
  EXPECT_EQ(backend.deletekey("key"), true);
  EXPECT_EQ(backend.deletekey("KASH"), true);
  EXPECT_EQ(backend.deletekey("EAT"), true);
  EXPECT_EQ(backend.deletekey("y"), true);
  EXPECT_EQ(backend.deletekey("12"), true);

  // attempt to get all keys (should return empty string)
  const std::string kEmptyString = "";
  EXPECT_EQ(backend.get("key"), kEmptyString);
  EXPECT_EQ(backend.get("KASH"), kEmptyString);
  EXPECT_EQ(backend.get("EAT"), kEmptyString);
  EXPECT_EQ(backend.get("y"), kEmptyString);
  EXPECT_EQ(backend.get("12"), kEmptyString);
}

// Deleting non-existant key should return false
TEST(KeyValueBackendTest, DeleteNonExistingKey) {
  KeyValueBackend backend;
  EXPECT_EQ(backend.deletekey("key"), false);
  EXPECT_EQ(backend.deletekey("KASH"), false);
  EXPECT_EQ(backend.deletekey("EAT"), false);
  EXPECT_EQ(backend.deletekey("y"), false);
  EXPECT_EQ(backend.deletekey("12"), false);
}

// Put, get, delete, get should not return value
TEST(KeyValueBackendTest, PutGetDelete) {
  KeyValueBackend backend;
  EXPECT_EQ(backend.deletekey("key"), false);
  EXPECT_EQ(backend.deletekey("KASH"), false);
  EXPECT_EQ(backend.deletekey("EAT"), false);
  EXPECT_EQ(backend.deletekey("y"), false);
  EXPECT_EQ(backend.deletekey("12"), false);
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
