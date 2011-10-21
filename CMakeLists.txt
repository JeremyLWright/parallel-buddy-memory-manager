CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(ParallelBinaryBuddy)
find_package(GTest)

SET(CMAKE_CXX_FLAGS_DEBUG "-g3 -ggdb -Wall -fopenmp")
include_directories(.)

add_executable(buddyDriver main.cpp)

if(GTEST_FOUND)
    add_custom_target(test 
        COMMAND test_buddy
        DEPENDS test_buddy)
    add_executable(test_buddy Tests/TestBuddyAllocator.cpp)
    target_link_libraries(test_buddy ${GTEST_BOTH_LIBRARIES} pthread)

endif()
