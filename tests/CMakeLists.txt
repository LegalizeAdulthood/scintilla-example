include(GoogleTest)

find_package(GTest CONFIG REQUIRED)
find_package(wxWidgets CONFIG REQUIRED)

add_executable(test-lexer
    lexer_test.cpp)
source_group("CMake Templates" REGULAR_EXPRESSION ".*\\.in$")
target_include_directories(test-lexer PRIVATE
    "${CMAKE_SOURCE_DIR}/scintilla/include")     # For access to ILexer, IDocument interfaces
target_link_libraries(test-lexer PUBLIC formula-syntax GTest::gmock_main wx::base)
target_folder(test-lexer "Tests")
target_copy_lexer_plugin(test-lexer)

gtest_discover_tests(test-lexer)
