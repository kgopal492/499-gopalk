#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "sl_functionality.h"
#include "ServiceLayer.pb.h"

// register new user returns true
TEST(ServiceLayerFunctionalityTest, ) {
  ServiceLayerFunctionality sl_func(true);

}
// register existing user returns false

// register user with same name, but different case returns true

// chirp return chirp with values filled

// add a couple of chirps, chirp value is correct

// follow user, monitor, user chirps, monitor should return the chirp

// multiple users follow a user, monitor, user chirps, should print with monitor

// create one chirp, read should return chirp

// create nested chirps, read should return nested chirps

// create nested chirps and read lower chirp, should return only lower chirps

// user follows multiple other users, monitor responds accordingly

// user follows no other user, monitor doesn't responds

// following user chirps, then user monitors, initial chirp not returned

// user exists returns true for user that user_exists

// user exists is case sensitive

// user exists returns false when user is not in database

// -1 is not invalid parent id

// parent_id larger than number of chirps invalid_argument

// parent_id within number of chirps is valid_parent_id

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
