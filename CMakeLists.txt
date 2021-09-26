cmake_minimum_required(VERSION 2.8)

project(ntrip_client)

add_compile_options(-std=c++11)
find_package(Threads REQUIRED)

include_directories(
    include
    )
add_executable(${PROJECT_NAME} main.cpp ntrip_client.cpp base64.cpp)

if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(${PROJECT_NAME} PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
endif()
