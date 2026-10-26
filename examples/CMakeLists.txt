add_executable(jest_cpp_basics basics.cpp)
target_link_libraries(jest_cpp_basics PRIVATE JestCpp::JestCpp)

add_executable(jest_cpp_hooks hooks.cpp)
target_link_libraries(jest_cpp_hooks PRIVATE JestCpp::JestCpp)