include(GoogleTest)

find_package(GTest CONFIG REQUIRED)
find_package(wxWidgets CONFIG REQUIRED)

file(GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/config.h"
    INPUT config.h.in
)

add_executable(test-lexer
    config.h.in
    "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/config.h"
    lexer_test.cpp)
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    source_group("Header Files/${config}" REGULAR_EXPRESSION ".*/${config}/[^/]*\\.h$")
endforeach()
source_group("CMake Templates" REGULAR_EXPRESSION ".*\\.in$")
target_include_directories(test-lexer PRIVATE
    "${CMAKE_SOURCE_DIR}/scintilla/include"     # For access to ILexer, IDocument interfaces
    "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>")
target_link_libraries(test-lexer PUBLIC GTest::gtest_main wx::base)
target_folder(test-lexer "Tests")
target_copy_lexer_plugin(test-lexer formula-lexer)

gtest_discover_tests(test-lexer)
