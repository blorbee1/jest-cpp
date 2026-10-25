add_executable(jest_cpp_tests main.cpp)

target_link_libraries(jest_cpp_tests PRIVATE JestCpp::JestCpp)

add_test(NAME jest_cpp_tests COMMAND jest_cpp_tests)