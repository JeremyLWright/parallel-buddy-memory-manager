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


if(GTEST_FOUND)
    ENABLE_TESTING()
    set(Boost_USE_STATIC_LIBS ON)
    Find_Package(Boost REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})
    add_custom_target(gtest 
        COMMAND test_buddy
        DEPENDS test_buddy)
    FILE(GLOB Test_Sources Tests/*.cpp)
    
    #Mutex Version
    add_executable(test_buddy ${Test_Sources})
    #Spinlock Version
    add_executable(test_buddy_spin ${Test_Sources})
    SET_TARGET_PROPERTIES(test_buddy_spin PROPERTIES COMPILE_DEFINITIONS SPIN_LOCK)
    
    target_link_libraries(test_buddy ${GTEST_BOTH_LIBRARIES} pthread boost_timer)
    target_link_libraries(test_buddy_spin ${GTEST_BOTH_LIBRARIES} pthread boost_timer)
    

    ADD_TEST(GTest ${CMAKE_CURRENT_BINARY_DIR}/test_buddy --gtest_output=xml)
    ADD_TEST(RunTest ${CMAKE_CURRENT_BINARY_DIR}/buddyDriver 5)

    add_custom_target(coverage
        COMMAND gcov -o ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/test_buddy.dir/Tests/TestBuddyAllocator.cpp.gcno ../Tests/TestBuddyAllocator.cpp
        DEPENDS test_buddy)

endif()
