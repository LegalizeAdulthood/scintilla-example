# Copy the lexer plugin to the target's executable directory
function(target_copy_lexer_plugin target)
    add_custom_command(TARGET formula-lexer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:formula-lexer> $<TARGET_FILE_DIR:${target}>
        COMMAND_EXPAND_LISTS
    )
endfunction()

add_library(formula-lexer SHARED
    lexer.cpp
)
target_include_directories(formula-lexer PRIVATE include)
target_link_libraries(formula-lexer PRIVATE lexlib)
target_folder(formula-lexer "Plug-Ins")
target_prefix(formula-lexer "")
target_copy_lexer_plugin(test-lexer)
target_copy_lexer_plugin(scintilla-example)
