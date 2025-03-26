find_package(Scintilla 3.7.2 EXACT REQUIRED)

add_library(lexer
    include/lexer/lexer.h
    lexer.cpp
)
target_include_directories(lexer PUBLIC include)
target_link_libraries(lexer PUBLIC Scintilla::Scintilla)
target_folder(lexer "Libraries")
