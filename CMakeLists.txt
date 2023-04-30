cmake_minimum_required(VERSION 3.5)

project(insidejob
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "Inside Job is a package that \
        manages the transfer of messages within a single process \
        between threads using pointers and callbacks"
)

set(CMAKE_CXX_STANDARD 17)

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

option(BROKER_QUEUE_SIZE "Set the broker queue size")

if(BROKER_QUEUE_SIZE)
    add_definitions(-DBROKER_QUEUE_SIZE=${BROKER_QUEUE_SIZE})
endif()

find_package(Threads REQUIRED)

include_directories(include)

set(SOURCES
    test/main.cpp
    src/broker.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
