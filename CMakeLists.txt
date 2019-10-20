cmake_minimum_required(VERSION 3.0)
project(tests)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
 
# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})


# Link runTests with what we want to test and the GTest and pthread library
add_executable(string_lib_tests src/tests/string_lib_tests.cpp)
target_link_libraries(string_lib_tests ${GTEST_LIBRARIES} pthread)

add_executable(scanner_tests src/tests/scanner_tests.cpp src/scanner.cpp src/string_lib.cpp)
target_link_libraries(scanner_tests ${GTEST_LIBRARIES} pthread)




