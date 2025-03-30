find_package(wxWidgets CONFIG REQUIRED)

add_executable(scintilla-example WIN32 main.cpp)
target_link_libraries(scintilla-example PUBLIC wx::stc wx::core wx::base)
target_folder(scintilla-example "Tools")

target_copy_lexer_plugin(scintilla-example)
