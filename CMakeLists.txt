cmake_minimum_required(VERSION 3.12)
project(glin)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake) # find geos path
if(APPLE) # This has to be manually setup
#    set(Boost_INCLUDE_DIR /usr/local/Cellar/boost/1.75.0/include)
    set(CMAKE_FIND_FRAMEWORK LAST)
    set(CMAKE_FIND_APPBUNDLE LAST)
endif()
set(Boost_USE_MULTITHREADED ON)
# FindGEOS.cmake is copied from https://github.com/valhalla/valhalla/blob/master/cmake/FindGEOS.cmake
find_package(GEOS REQUIRED)# if not find geo, cannot compile find geos.cmake
find_package(Boost REQUIRED)
set(CMAKE_CXX_STANDARD 14)


# SIMD
if(MSVC)
    set(CMAKE_CXX_FLAGS "/O2 /arch:AVX2 /W1 /EHsc")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "-O3 -xHost")
else()
    # clang and gcc
    set(CMAKE_CXX_FLAGS "-O3 -march=native -Wall -Wextra")
endif()


add_executable(test_glin test/unittest_main.cpp glin/hilbert/hilbert.h glin/hilbert/hilbert.cpp)
target_link_libraries(test_glin PRIVATE ${GEOS_LIBRARY} ${Boost_SYSTEM_LIBRARY})
target_include_directories(test_glin PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/core;${GEOS_INCLUDE_DIR};${Boost_INCLUDE_DIRS}>")

add_executable(test_glin_piece test/unittest_main.cpp glin/hilbert/hilbert.h glin/hilbert/hilbert.cpp)
target_compile_definitions(test_glin_piece PRIVATE PIECE)
target_link_libraries(test_glin_piece PRIVATE ${GEOS_LIBRARY} ${Boost_SYSTEM_LIBRARY})
target_include_directories(test_glin_piece PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/core;${GEOS_INCLUDE_DIR};${Boost_INCLUDE_DIRS}>")

enable_testing()
add_test(test_glin test_glin_piece) #running test while compile