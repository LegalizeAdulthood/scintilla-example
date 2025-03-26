include(FindPackageHandleStandardArgs)

find_file(ILEXER_H
    NAMES ILexer.h
    PATHS "${Scintilla_ROOT}/include" "${CMAKE_SOURCE_DIR}/scintilla/include"
    NO_DEFAULT_PATH
)

if(ILEXER_H)
    get_filename_component(Scintilla_INCLUDE_DIR ${ILEXER_H} DIRECTORY)
    set(Scintilla_FOUND TRUE)
endif()

if(Scintilla_FOUND AND Scintilla_FIND_VERSION)
    find_file(VERSION_TXT
        NAMES version.txt
        PATHS "${Scintilla_ROOT}" "${Scintilla_INCLUDE_DIR}/.."
        NO_DEFAULT_PATH
    )
    if(VERSION_TXT)
        file(READ "${VERSION_TXT}" Scintilla_VERSION_CONTENTS)
        string(REGEX MATCH "([0-9])([0-9])([0-9])" Scintilla_VERSION_MATCHES "${Scintilla_VERSION_CONTENTS}")
        set(Scintilla_VERSION "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")
        if(Scintilla_VERSION VERSION_LESS "${Scintilla_FIND_VERSION}")
            set(Scintilla_FOUND FALSE)
        endif()
    else()
        set(Scintilla_FOUND FALSE)
    endif()
endif()

if(Scintilla_FOUND)
    add_library(Scintilla INTERFACE)
    target_include_directories(Scintilla INTERFACE ${Scintilla_INCLUDE_DIR})
    add_library(Scintilla::Scintilla ALIAS Scintilla)
else()
    set(Scintilla_INCLUDE_DIR)
endif()

find_package_handle_standard_args(Scintilla DEFAULT_MSG Scintilla_INCLUDE_DIR)
