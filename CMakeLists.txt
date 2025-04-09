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

add_custom_target(Miscelleanous)
target_sources(Miscelleanous PRIVATE
    ".clang-format"
    ".editorconfig"
    ".gitignore"
    ".gitmodules"
    ".github/workflows/cmake.yml"
    "CMakePresets.json"
    "ReadMe.md"
    "vcpkg.json"
    "vcpkg-configuration.json"
    "cmake/cxx_standard_17.cmake"
    "cmake/target_copy_lexer_plugin.cmake"
    "cmake/target_folder.cmake"
    "cmake/target_prefix.cmake"
    "cmake/vs_startup_project.cmake"
)
source_group("CMake Scripts" FILES
    "cmake/cxx_standard_17.cmake"
    "cmake/target_copy_lexer_plugin.cmake"
    "cmake/target_folder.cmake"
    "cmake/target_prefix.cmake"
    "cmake/vs_startup_project.cmake")
source_group("Editing" FILES
    ".clang-format"
    ".editorconfig")
source_group("Git" FILES
    ".gitignore"
    ".gitmodules")
source_group("GitHub" FILES
    ".github/workflows/cmake.yml")
source_group("VcPkg" FILES
    "vcpkg.json"
    "vcpkg-configuration.json")
