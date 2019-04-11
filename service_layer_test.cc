#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "sl_functionality.h"
#include "ServiceLayer.pb.h"

// register new user returns true
TEST(ServiceLayerFunctionalityTest, RegisterNewUser) {
  ServiceLayerFunctionality sl_func(true);
  EXPECT_EQ(sl_func.registeruser("krishna"), true);
}
// register existing user returns false
TEST(ServiceLayerFunctionalityTest, RegisterExistingUser) {
  ServiceLayerFunctionality sl_func(true);
  EXPECT_EQ(sl_func.registeruser("krishna"), true);
  EXPECT_EQ(sl_func.registeruser("krishna"), false);
}
// register user with same name, but different case returns true
TEST(ServiceLayerFunctionalityTest, RegisterDifferentCase) {
  ServiceLayerFunctionality sl_func(true);
  EXPECT_EQ(sl_func.registeruser("krishna"), true);
  EXPECT_EQ(sl_func.registeruser("Krishna"), true);
}
// chirp return chirp with values filled
TEST(ServiceLayerFunctionalityTest, ValidChirp) {
  ServiceLayerFunctionality sl_func(true);
  // store chirp data values
  Chirp *chirp = new Chirp();
  std::string username = "barath";
  std::string text = "hello class";
  std::string parent_id = "-1";
  sl_func.chirp(chirp, username, text, parent_id);
  // verify that the values remain the same
  EXPECT_EQ(chirp->username(), username);
  EXPECT_EQ(chirp->text(), text);
  EXPECT_EQ(chirp->id(), "0");
  EXPECT_EQ(chirp->parent_id(), parent_id);
  delete chirp;
}
// add a couple of chirps, chirp value is correct
// chirp return chirp with values filled
TEST(ServiceLayerFunctionalityTest, MultipleChirps) {
  ServiceLayerFunctionality sl_func(true);
  // store chirp data values
  Chirp *chirp1 = new Chirp();
  std::string username1 = "barath";
  std::string text1 = "hello class";
  std::string parent_id1 = "-1";
  sl_func.chirp(chirp1, username1, text1, parent_id1);
  // store chirp 2 data values
  Chirp *chirp2 = new Chirp();
  std::string username2 = "kamya";
  std::string text2 = "hey there!";
  std::string parent_id2 = "0";
  sl_func.chirp(chirp2, username2, text2, parent_id2);
  // chirp 1 should have the values of the first chirp
  EXPECT_EQ(chirp1->username(), username1);
  EXPECT_EQ(chirp1->text(), text1);
  EXPECT_EQ(chirp1->id(), "0");
  EXPECT_EQ(chirp1->parent_id(), parent_id1);
  // chirp 1 should have the values of the second chirp
  EXPECT_EQ(chirp2->username(), username2);
  EXPECT_EQ(chirp2->text(), text2);
  EXPECT_EQ(chirp2->id(), "1");
  EXPECT_EQ(chirp2->parent_id(), parent_id2);
}
// multiple users follow a user, monitor, user chirps, should print with monitor
TEST(ServiceLayerFunctionalityTest, MultipleFollowersMonitoring) {
  ServiceLayerFunctionality sl_func(true);
  // register users
  sl_func.registeruser("krishna");
  sl_func.registeruser("kam");
  sl_func.registeruser("sam");
  // 2 users follow one other
  sl_func.follow("krishna", "kam");
  sl_func.follow("sam", "kam");
  // have both users begin monitor
  Chirps chirps1 = sl_func.monitor("krishna");
  Chirps chirps2 = sl_func.monitor("sam");
  // monitor should not initially return a chirp
  EXPECT_EQ(chirps1.chirps_size(), 0);
  EXPECT_EQ(chirps2.chirps_size(), 0);
  // person 3 will now chirp
  Chirp *chirp = new Chirp();
  std::string username = "kam";
  std::string text = "hello class";
  std::string parent_id = "-1";
  sl_func.chirp(chirp, username, text, parent_id);
  // run monitor to collect chirps
  chirps1 = sl_func.monitor("krishna");
  chirps2 = sl_func.monitor("sam");
  // only one chirp should be expected to return
  EXPECT_EQ(chirps1.chirps_size(), 1);
  EXPECT_EQ(chirps2.chirps_size(), 1);
  // both monitor responses should have the same value
  EXPECT_EQ(chirps1.chirps(0).username(), username);
  EXPECT_EQ(chirps1.chirps(0).text(), text);
  EXPECT_EQ(chirps1.chirps(0).id(), "0");
  EXPECT_EQ(chirps1.chirps(0).parent_id(), parent_id);
  EXPECT_EQ(chirps2.chirps(0).username(), username);
  EXPECT_EQ(chirps2.chirps(0).text(), text);
  EXPECT_EQ(chirps2.chirps(0).id(), "0");
  EXPECT_EQ(chirps2.chirps(0).parent_id(), parent_id);
}

// user follows multiple other users, monitor responds accordingly
TEST(ServiceLayerFunctionalityTest, UserFollow) {
  ServiceLayerFunctionality sl_func(true);
  // register users
  sl_func.registeruser("krishna");
  sl_func.registeruser("kam");
  sl_func.registeruser("sam");
  // 2 users follow one other
  sl_func.follow("krishna", "kam");
  sl_func.follow("krishna", "sam");
  // have both users begin monitor
  Chirps chirps = sl_func.monitor("krishna");
  // monitor should not initially return a chirp
  EXPECT_EQ(chirps.chirps_size(), 0);

  // Following will chirp
  Chirp *chirp1 = new Chirp();
  std::string username1 = "kam";
  std::string text1 = "hello friends";
  std::string parent_id1 = "-1";
  sl_func.chirp(chirp1, username1, text1, parent_id1);
  // Following will chirp
  Chirp *chirp2 = new Chirp();
  std::string username2 = "sam";
  std::string text2 = "hey there";
  std::string parent_id2 = "0";
  sl_func.chirp(chirp2, username2, text2, parent_id2);
  // run monitor to collect chirps
  chirps = sl_func.monitor("krishna");
  // only one chirp should be expected to return
  EXPECT_EQ(chirps.chirps_size(), 2);
  // both monitor responses should have the same value
  EXPECT_EQ(chirps.chirps(0).username(), username1);
  EXPECT_EQ(chirps.chirps(0).text(), text1);
  EXPECT_EQ(chirps.chirps(0).id(), "0");
  EXPECT_EQ(chirps.chirps(0).parent_id(), parent_id1);
  EXPECT_EQ(chirps.chirps(1).username(), username2);
  EXPECT_EQ(chirps.chirps(1).text(), text2);
  EXPECT_EQ(chirps.chirps(1).id(), "1");
  EXPECT_EQ(chirps.chirps(1).parent_id(), parent_id2);
}

// following user chirps, then user monitors, initial chirp not returned

// create one chirp, read should return chirp
// create nested chirps, read should return nested chirps
// create nested chirps and read lower chirp
// user follows multiple other users, monitor responds accordingly
TEST(ServiceLayerFunctionalityTest, ReadThread) {
  ServiceLayerFunctionality sl_func(true);
  // register users
  sl_func.registeruser("krishna");
  sl_func.registeruser("kam");

  // Following will chirp
  Chirp *chirp1 = new Chirp();
  std::string username1 = "kam";
  std::string text1 = "hello friends";
  std::string parent_id1 = "-1";
  sl_func.chirp(chirp1, username1, text1, parent_id1);

  // read chirp1
  std::vector<Chirp> chirps = sl_func.read("0");
  EXPECT_EQ(chirps.size(), 1);
  EXPECT_EQ(chirps[0].username(), username1);
  EXPECT_EQ(chirps[0].text(), text1);
  EXPECT_EQ(chirps[0].id(), "0");
  EXPECT_EQ(chirps[0].parent_id(), parent_id1);

  // Following will chirp
  Chirp *chirp2 = new Chirp();
  std::string username2 = "krishna";
  std::string text2 = "hey there";
  std::string parent_id2 = "0";
  sl_func.chirp(chirp2, username2, text2, parent_id2);

  // read first chirp again
  chirps = sl_func.read("0");
  EXPECT_EQ(chirps.size(), 2);
  EXPECT_EQ(chirps[0].username(), username1);
  EXPECT_EQ(chirps[0].text(), text1);
  EXPECT_EQ(chirps[0].id(), "0");
  EXPECT_EQ(chirps[0].parent_id(), parent_id1);
  EXPECT_EQ(chirps[1].username(), username2);
  EXPECT_EQ(chirps[1].text(), text2);
  EXPECT_EQ(chirps[1].id(), "1");
  EXPECT_EQ(chirps[1].parent_id(), parent_id2);

  // read second chirp
  chirps = sl_func.read("1");
  EXPECT_EQ(chirps.size(), 1);
  EXPECT_EQ(chirps[0].username(), username2);
  EXPECT_EQ(chirps[0].text(), text2);
  EXPECT_EQ(chirps[0].id(), "1");
  EXPECT_EQ(chirps[0].parent_id(), parent_id2);
}
// tests user_exists function, should be case sensitive
// user exists returns false when user is not in database
TEST(ServiceLayerFunctionalityTest, UserExists) {
  ServiceLayerFunctionality sl_func(true);
  // test first, then register user
  EXPECT_EQ(sl_func.user_exists("krishna"), false);
  sl_func.registeruser("krishna");
  EXPECT_EQ(sl_func.user_exists("krishna"), true);
}

// -1 is not invalid parent id
// parent_id larger than number of chirps invalid_argument
// parent_id within number of chirps is valid_parent_id
TEST(ServiceLayerFunctionalityTest, ValidParentID) {
  ServiceLayerFunctionality sl_func(true);
  // -1 is default parent_id for people who are not replying
  EXPECT_EQ(sl_func.valid_parent_id("-1"), true);
  // 0, 5 should return false since there has been no tweets
  EXPECT_EQ(sl_func.valid_parent_id("0"), false);
  EXPECT_EQ(sl_func.valid_parent_id("5"), false);
}
// tests stream function with a basic hashtag
// ensures that a new chirp is streamed if it has the correct hashtag
// ensures that streaming stops and is cleaned up after the user stops streaming
TEST(ServiceLayerFunctionalityTest, Stream) {
  ServiceLayerFunctionality sl_func(true);
  sl_func.registeruser("jillian");
  EXPECT_EQ(sl_func.user_exists("jillian"), true);
  sl_func.start_stream("jillian", "#Athenahacks");

  Chirps chirps1 = sl_func.stream("jillian");
  // stream should not initially return a chirp
  EXPECT_EQ(chirps1.chirps_size(), 0);

  Chirp *chirp1 = new Chirp();
  std::string username1 = "krishna";
  std::string text1 = "I know that #Athenahacks is awesome";
  std::string parent_id1 = "-1";
  sl_func.chirp(chirp1, username1, text1, parent_id1);

  chirps1 = sl_func.stream("jillian");
  EXPECT_EQ(chirps1.chirps_size(), 1);
  chirps1 = sl_func.stream("jillian");
  EXPECT_EQ(chirps1.chirps_size(), 0);

  sl_func.end_stream("jillian", "#Athenahacks");
  Chirp *chirp2 = new Chirp();
  std::string username2 = "krishna";
  std::string text2 = "#Athenahacks";
  std::string parent_id2 = "-1";
  sl_func.chirp(chirp2, username2, text2, parent_id2);
  Chirps chirps2 = sl_func.stream("jillian");
  EXPECT_EQ(chirps2.chirps_size(), 0);

  delete chirp1;
  delete chirp2;
}

// tests stream function with a chirp with a streaming hashtag as a substring
// that chirp should not be streamed to the user streaming on the substring
// hashtag
TEST(ServiceLayerFunctionalityTest, StreamInvalid) {
  ServiceLayerFunctionality sl_func(true);
  sl_func.registeruser("jillian");
  EXPECT_EQ(sl_func.user_exists("jillian"), true);
  sl_func.start_stream("jillian", "#Athenahacks");

  Chirp *chirp1 = new Chirp();
  std::string username1 = "krishna";
  std::string text1 = "#Athenahacks2019";
  std::string parent_id1 = "-1";
  sl_func.chirp(chirp1, username1, text1, parent_id1);

  Chirps chirps1 = sl_func.stream("jillian");
  EXPECT_EQ(chirps1.chirps_size(), 0);

  sl_func.end_stream("jillian", "#Athenahacks");

  delete chirp1;
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
