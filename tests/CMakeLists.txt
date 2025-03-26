include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

add_executable(test-lexer lexer_test.cpp)
target_link_libraries(test-lexer PUBLIC lexer GTest::gtest_main)
target_folder(test-lexer "Tests")

gtest_discover_tests(test-lexer)
