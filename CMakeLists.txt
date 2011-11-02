CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(ParallelBinaryBuddy)
find_package(GTest)
find_package(Boost)
SET(CMAKE_BUILD_TYPE DEBUG)
SET(CMAKE_CXX_FLAGS_DEBUG "-g3 -ggdb -Wall -fopenmp")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -fopenmp")
include_directories(.)

add_executable(buddyDriver Driver/main.cpp)

if(GTEST_FOUND)
    add_custom_target(test 
        COMMAND test_buddy
        DEPENDS test_buddy)
    add_executable(test_buddy Tests/TestBuddyAllocator.cpp)
    target_link_libraries(test_buddy ${GTEST_BOTH_LIBRARIES} pthread)

endif()
