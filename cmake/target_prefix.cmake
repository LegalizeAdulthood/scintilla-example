function(target_prefix target prefix)
    set_target_properties("${target}" PROPERTIES PREFIX "${prefix}")
endfunction()
