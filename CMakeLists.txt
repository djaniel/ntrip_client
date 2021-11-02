cmake_minimum_required(VERSION 3.5)
project(libntripclient VERSION 1.0.0 LANGUAGES CXX)
add_compile_options(-std=c++11)

##### System dependencies
find_package(Threads REQUIRED)
find_package(Catch2 REQUIRED)

##### Create target
add_library(ntripclient 
   src/ntrip_client.cpp
   src/base64.cpp
   src/base64.h
)

##### Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(NTRIPClient::ntripclient ALIAS ntripclient)

##### Set target properties
target_include_directories(ntripclient
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(ntripclient PRIVATE cxx_auto_type)
target_link_libraries(ntripclient PRIVATE Threads::Threads)

add_executable(ntripclient_test test/src/main_test.cpp)

target_link_libraries(ntripclient_test PRIVATE Catch2::Catch2WithMain)

#target_compile_options(ntripclient PUBLIC $<$<OR:$<COMPILER_ID:GNU>,$<COMPILER_ID:Clang>>:-pthread>)

#target_compile_options(ntripclient PUBLIC "$<$<CONFIG:DEBUG>:${MY_DEBUG_OPTIONS}>")
#target_compile_options(ntripclient PRIVATE "-Wall -Wextra -Wpedantic -pthread")

#target_link_libraries(jsonutils
#    PUBLIC
#        Boost::headers RapidJSON::RapidJSON
#    PRIVATE
#        Boost::regex
#)

#include_directories(
#    include
#    )
#    
#
#
#add_executable(${PROJECT_NAME} main.cpp ntrip_client.cpp base64.cpp)
#
#if(THREADS_HAVE_PTHREAD_ARG)
#    target_compile_options(${PROJECT_NAME} PUBLIC "-pthread")
#endif()
#if(CMAKE_THREAD_LIBS_INIT)
#    target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
#endif()
