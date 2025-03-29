add_library(lexer
    include/lexer/lexer.h
    lexer.cpp
)
target_include_directories(lexer PUBLIC include)
target_link_libraries(lexer PUBLIC lexlib)
target_folder(lexer "Libraries")
