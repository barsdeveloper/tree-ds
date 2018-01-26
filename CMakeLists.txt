cmake_minimum_required(VERSION 2.6)
project(TreeDS)
set(PROJECT_VERSION 1.0)

include(CTest)
enable_testing(true)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall --coverage")

include_directories("./include")
find_package(Qt5Test REQUIRED)
add_executable(tests "./test/TreeIterationTest.cpp")
target_link_libraries(tests Qt5::Test)
add_test("IterationTest" tests)

add_custom_target(
    SonarQube
    # coverage generation
    COMMAND kcov --include-path=${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/coverage ${CMAKE_BINARY_DIR}/tests
    # sonar scan
    COMMAND sonar-scanner
    DEPENDS tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)