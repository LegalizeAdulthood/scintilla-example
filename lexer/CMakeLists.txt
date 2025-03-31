add_library(formula-syntax INTERFACE include/formula/syntax.h)
target_include_directories(formula-syntax INTERFACE include)
target_folder(formula-syntax "Libraries")

add_library(formula-lexer SHARED
    lexer.cpp
)
target_link_libraries(formula-lexer PRIVATE lexlib formula-syntax)
target_folder(formula-lexer "Plug-Ins")
target_prefix(formula-lexer "")
