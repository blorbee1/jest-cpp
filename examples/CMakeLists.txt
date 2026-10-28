add_executable(jest_cpp_basics basics.cpp)
target_link_libraries(jest_cpp_basics PRIVATE JestCpp::JestCpp)

add_executable(jest_cpp_hooks hooks.cpp)
target_link_libraries(jest_cpp_hooks PRIVATE JestCpp::JestCpp)

add_executable(jest_cpp_each_and_failing each_and_failing.cpp)
target_link_libraries(jest_cpp_each_and_failing PRIVATE JestCpp::JestCpp)

add_executable(jest_cpp_matchers matchers.cpp)
target_link_libraries(jest_cpp_matchers PRIVATE JestCpp::JestCpp)

add_executable(jest_cpp_mocks mocks.cpp)
target_link_libraries(jest_cpp_mocks PRIVATE JestCpp::JestCpp)
