# Copy the lexer plugin to the target's executable directory
function(target_copy_lexer_plugin target lexer)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${lexer}> $<TARGET_FILE_DIR:${target}>
        COMMAND_EXPAND_LISTS
    )
    add_dependencies(${target} ${lexer})
endfunction()
