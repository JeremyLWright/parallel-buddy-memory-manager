CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(ParallelBinaryBuddy)
find_package(GTest)
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -ggdb -Wall -fprofile-arcs -ftest-coverage -fopenmp -fpermissive")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -fopenmp")
SET(CMAKE_CXX_FLAGS_RELWITHEBINFO "-O2 -DNDEBUG -pg -fopenmp")
SET(LDFLAGS "-fprofile-arcs -ftest-coverage")
include_directories(.)

add_executable(buddyDriver Driver/main.cpp)

if(GTEST_FOUND)
    ENABLE_TESTING()
    add_custom_target(gtest 
        COMMAND test_buddy
        DEPENDS test_buddy)
    add_executable(test_buddy Tests/TestBuddyAllocator.cpp)
    target_link_libraries(test_buddy ${GTEST_BOTH_LIBRARIES} pthread gomp)
    ADD_TEST(SimpleTest ${CMAKE_CURRENT_BINARY_DIR}/test_buddy --gtest_output=xml)

    add_custom_target(coverage
        COMMAND gcov -o ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_buddy.dir/Tests/TestBuddyAllocator.cpp.gcno ../Tests/TestBuddyAllocator.cpp
        DEPENDS test_buddy)

endif()
