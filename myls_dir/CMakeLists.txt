cmake_minimum_required(VERSION 3.9)
project(myls)

FIND_PACKAGE( Boost COMPONENTS filesystem system REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

set(CMAKE_CXX_STANDARD 14)

add_executable(myls myls.cpp)
TARGET_LINK_LIBRARIES( myls LINK_PUBLIC ${Boost_LIBRARIES} )