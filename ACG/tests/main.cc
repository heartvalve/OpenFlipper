/*
 * main.cc
 *
 *  Created on: May 18, 2012
 *      Author: ebke
 */


#include "gtest/gtest.h"


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
