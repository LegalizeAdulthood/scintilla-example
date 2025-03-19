include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

add_executable(test-project-template test.cpp)
target_link_libraries(test-project-template PUBLIC GTest::gtest_main)
target_folder(test-project-template "Tests")

gtest_discover_tests(test-project-template)
