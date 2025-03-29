set(Scintilla_ROOT ${CMAKE_SOURCE_DIR}/scintilla)

set(LEXLIB_DIR ${Scintilla_ROOT}/lexlib)
add_library(lexlib STATIC
    ${LEXLIB_DIR}/Accessor.h
    ${LEXLIB_DIR}/CharacterCategory.h
    ${LEXLIB_DIR}/CharacterSet.h
    ${LEXLIB_DIR}/LexAccessor.h
    ${LEXLIB_DIR}/LexerBase.h
    ${LEXLIB_DIR}/LexerModule.h
    ${LEXLIB_DIR}/LexerNoExceptions.h
    ${LEXLIB_DIR}/LexerSimple.h
    ${LEXLIB_DIR}/OptionSet.h
    ${LEXLIB_DIR}/PropSetSimple.h
    ${LEXLIB_DIR}/SparseState.h
    ${LEXLIB_DIR}/StringCopy.h
    ${LEXLIB_DIR}/StyleContext.h
    ${LEXLIB_DIR}/SubStyles.h
    ${LEXLIB_DIR}/WordList.h
    ${LEXLIB_DIR}/Accessor.cxx
    ${LEXLIB_DIR}/CharacterCategory.cxx
    ${LEXLIB_DIR}/CharacterSet.cxx
    ${LEXLIB_DIR}/LexerBase.cxx
    ${LEXLIB_DIR}/LexerModule.cxx
    ${LEXLIB_DIR}/LexerNoExceptions.cxx
    ${LEXLIB_DIR}/LexerSimple.cxx
    ${LEXLIB_DIR}/PropSetSimple.cxx
    ${LEXLIB_DIR}/StyleContext.cxx
    ${LEXLIB_DIR}/WordList.cxx
)
target_include_directories(lexlib PUBLIC ${LEXLIB_DIR} ${Scintilla_ROOT}/include)
target_folder(lexlib "Libraries")
