include(GoogleTest)

find_package(GTest CONFIG REQUIRED)
find_package(wxWidgets CONFIG REQUIRED)

add_executable(test-lexer lexer_test.cpp)
# For access to ILexer, IDocument interfaces
target_include_directories(test-lexer PRIVATE "${CMAKE_SOURCE_DIR}/scintilla/include")
target_link_libraries(test-lexer PUBLIC GTest::gtest_main wx::core wx::base)
target_folder(test-lexer "Tests")
target_copy_lexer_plugin(test-lexer formula-lexer)

gtest_discover_tests(test-lexer)
