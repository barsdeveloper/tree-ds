cmake_minimum_required(VERSION 3.8)
project(TreeDS)
set(PROJECT_VERSION 1.0)

#   ---   REPLACE WITH YOURS   ---   
set(KCOV_COMMAND kcov)
set(SONAR_SCANNER_COMMAND ./sonar.sh)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "--coverage -g3 -O0 -Wall")

include(CTest)
enable_testing(true)

include_directories("./include")
find_package(Qt5Test REQUIRED)
file(GLOB TEST_SOURCES test/*.cpp)# get files from test and make a list TEST_SOURCES
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} Qt5::Test)
    add_test(${TEST_NAME} ${TEST_NAME})
endforeach(TEST_SOURCE ${TEST_SOURCES})

add_custom_target(SonarQube)

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_custom_command(
        TARGET SonarQube
        PRE_BUILD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Collecting coverage data for \"${TEST_NAME}\"."
        COMMAND ${KCOV_COMMAND} --include-path=${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/coverage ${CMAKE_BINARY_DIR}/${TEST_NAME}
    )
endforeach(TEST_SOURCE ${TEST_SOURCES})

add_custom_command(
    TARGET SonarQube
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Calling the sonar scanner (\"sonar.sh\")"
    COMMAND ${SONAR_SCANNER_COMMAND}
)
