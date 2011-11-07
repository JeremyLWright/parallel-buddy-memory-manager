CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(ParallelBinaryBuddy)
find_package(GTest)
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -ggdb -Wall -fprofile-arcs -ftest-coverage -fopenmp -fpermissive")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -fopenmp -fpermissive")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -pg -fpermissive -DINSTRUMENT")
SET(LDFLAGS "-fprofile-arcs -ftest-coverage")
include_directories(.)

add_executable(buddyDriver Driver/main.cpp Driver/SieveOfAtkin.cpp)
target_link_libraries(buddyDriver pthread)


#Build All the instrumentation executables
IF(${CMAKE_BUILD_TYPE} MATCHES RelWithDebInfo)
    add_executable(byteSizeSpeed Instrumentation/bitspeed.cpp)
    target_link_libraries(byteSizeSpeed pthread)
    add_executable(hwordSizeSpeed Instrumentation/blocksizeVsRequestsPerSec.cpp)
    target_link_libraries(hwordSizeSpeed pthread)
    add_executable(wordSizeSpeed Instrumentation/dwordspeed.cpp)
    target_link_libraries(wordSizeSpeed pthread)
    add_executable(dwordSizeSpeed Instrumentation/halfwordspeed.cpp)
    target_link_libraries(dwordSizeSpeed pthread)
    add_executable(bitSizeSpeed Instrumentation/wordspeed.cpp)
    target_link_libraries(bitSizeSpeed pthread)
    add_executable(platformSpeed Instrumentation/platform.cpp)
    target_link_libraries(platformSpeed pthread)
ENDIF()




if(GTEST_FOUND)
    ENABLE_TESTING()
    add_custom_target(gtest 
        COMMAND test_buddy
        DEPENDS test_buddy)
    add_executable(test_buddy Tests/TestBuddyAllocator.cpp)
    target_link_libraries(test_buddy ${GTEST_BOTH_LIBRARIES} pthread gomp)
    ADD_TEST(GTest ${CMAKE_CURRENT_BINARY_DIR}/test_buddy --gtest_output=xml)
    ADD_TEST(RunTest ${CMAKE_CURRENT_BINARY_DIR}/buddyDriver 5)

    add_custom_target(coverage
        COMMAND gcov -o ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_buddy.dir/Tests/TestBuddyAllocator.cpp.gcno ../Tests/TestBuddyAllocator.cpp
        DEPENDS test_buddy)

endif()
