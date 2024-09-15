cmake_minimum_required(VERSION 3.14)
project(cpp-bson-convert)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
# Find the required packages
find_package(mongocxx CONFIG REQUIRED)

# Add the include directory
include_directories(${PROJECT_SOURCE_DIR}/src)

add_library(cpp-bson-convert INTERFACE
        src/cpp-bson-convert.hpp)
target_include_directories(cpp-bson-convert INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_sources(cpp-bson-convert INTERFACE ${PROJECT_SOURCE_DIR}/src/cpp-bson-convert.hpp)

target_link_libraries(cpp-bson-convert INTERFACE
        mongo::bsoncxx_static
        mongo::mongocxx_static)


add_subdirectory(test)