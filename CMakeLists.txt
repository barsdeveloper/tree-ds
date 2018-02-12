cmake_minimum_required(VERSION 3.1)
project(TreeDS)
set(PROJECT_VERSION 1.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall --coverage")

include(CTest)
enable_testing(true)

set(CMAKE_PREFIX_PATH "~/Qt/5.10.0/gcc_64/")# set this to the qt installation path
include_directories("./include")
find_package(Qt5Test REQUIRED)
file(GLOB TEST_SOURCES test/*.cpp)# get files from test and make a list TEST_SOURCES
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} Qt5::Test)
    add_test(${TEST_NAME} ${TEST_NAME})
endforeach(TEST_SOURCE ${TEST_SOURCES})

add_custom_target(
    SonarQube
    # coverage generation
    COMMAND kcov --include-path=${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/coverage ${CMAKE_BINARY_DIR}/tests
    # sonar scan
    COMMAND sonar-scanner
    DEPENDS tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)