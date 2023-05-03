cmake_minimum_required(VERSION 3.5)

project(insidejob
    VERSION 0.0.2
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
    src/broker.cpp
)

add_executable(tests_runner "test/tests_runner.cpp")
set(TEST_SOURCES
    test/test_broker.cpp
    test/test_hashmap.cpp
)

set(TESTS "")

foreach(TEST ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST} NAME_WLE)
    add_executable(${TEST_NAME} ${SOURCES} ${TEST})
    target_link_libraries(${TEST_NAME} PRIVATE Threads::Threads)
    list(APPEND TESTS ${TEST_NAME})
endforeach()

add_custom_target(
    test
    COMMAND tests_runner ${TESTS}
    COMMENT "Running custom commands"
    DEPENDS ${TESTS}
)