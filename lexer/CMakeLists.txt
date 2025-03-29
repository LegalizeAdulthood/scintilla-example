add_library(formula-lexer SHARED
    lexer.cpp
)
target_include_directories(formula-lexer PRIVATE include)
target_link_libraries(formula-lexer PRIVATE lexlib)
target_folder(formula-lexer "Plug-Ins")
