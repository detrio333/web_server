cmake_minimum_required(VERSION 2.8)

project(final)
include_directories(include)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package( Threads ) 

set(SRC_EXE src/main.c 
            src/arguments.c 
            src/daemonize.c
            src/logger.c
            src/server.c
            src/server_lowlvl.c
            src/ioservice.c
            src/data_service.c)


add_executable(final ${SRC_EXE})

target_link_libraries(final ${CMAKE_THREAD_LIBS_INIT})
