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

find_package(Threads REQUIRED)

include_directories(include)

set(SOURCES 
    src/main.cpp
    src/broker.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
