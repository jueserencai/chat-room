#include "gtest/gtest.h"

extern "C" {
#include "client/user.h"
}

TEST(user, signup) {
    ASSERT_TRUE(sign_up());
}