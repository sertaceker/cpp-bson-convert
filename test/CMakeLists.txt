cmake_minimum_required(VERSION 3.14)

project(test)

find_package(GTest CONFIG REQUIRED)

add_executable(test test.cpp)

target_link_libraries(test PRIVATE
        GTest::gtest
        GTest::gtest_main
        mongo::bsoncxx_static
        mongo::mongocxx_static
        cpp-bson-convert)