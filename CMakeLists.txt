cmake_minimum_required(VERSION 3.23)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(cxx_standard_17)
include(target_copy_lexer_plugin)
include(target_folder)
include(target_prefix)
include(vs_startup_project)

set(SCINTILLA_PROJECT_NAME "Scintilla Example" CACHE "STRING" "Default project name for IDEs")
project("${SCINTILLA_PROJECT_NAME}" CXX)

include(CTest)

add_subdirectory(lexlib)
add_subdirectory(lexer)
add_subdirectory(tools)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT scintilla-example)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

vs_startup_project(scintilla-example)
