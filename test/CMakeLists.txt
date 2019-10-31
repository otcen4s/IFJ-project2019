add_executable(scanner_test scanner_test.c) 
add_library(scanner STATIC ../src/scanner.c)
add_library(string_lib STATIC ../src/string_lib.c)
target_link_libraries(scanner_test unity scanner string_lib) 
add_test(scanner_test ${CMAKE_BINARY_DIR}/bin/scanner_test)

add_executable(string_lib_test string_lib_test.c) 
target_link_libraries(string_lib_test unity string_lib) 
add_test(string_lib_test ${CMAKE_BINARY_DIR}/bin/string_lib_test)
