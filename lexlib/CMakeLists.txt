set(Scintilla_ROOT ${CMAKE_SOURCE_DIR}/scintilla)

set(SCINTILLA_DIR ${Scintilla_ROOT}/include)
add_library(Scintilla INTERFACE
    ${SCINTILLA_DIR}/ILexer.h
    ${SCINTILLA_DIR}/Platform.h
    ${SCINTILLA_DIR}/SciLexer.h
    ${SCINTILLA_DIR}/Scintilla.h
    ${SCINTILLA_DIR}/ScintillaWidget.h
    ${SCINTILLA_DIR}/Sci_Position.h
)
target_include_directories(Scintilla INTERFACE ${Scintilla_ROOT}/include)
target_folder(Scintilla "Libraries")

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
target_include_directories(lexlib PUBLIC ${LEXLIB_DIR})
target_link_libraries(lexlib PUBLIC Scintilla)
target_folder(lexlib "Libraries")

option(BUILD_EXAMPLE_LEXERS ON)
if(BUILD_EXAMPLE_LEXERS)
    set(LEXERS_DIR "${Scintilla_ROOT}/lexers")
    add_library(lexer-examples STATIC
        ${LEXERS_DIR}/LexA68k.cxx
        ${LEXERS_DIR}/LexAbaqus.cxx
        ${LEXERS_DIR}/LexAda.cxx
        ${LEXERS_DIR}/LexAPDL.cxx
        ${LEXERS_DIR}/LexAsm.cxx
        ${LEXERS_DIR}/LexAsn1.cxx
        ${LEXERS_DIR}/LexASY.cxx
        ${LEXERS_DIR}/LexAU3.cxx
        ${LEXERS_DIR}/LexAVE.cxx
        ${LEXERS_DIR}/LexAVS.cxx
        ${LEXERS_DIR}/LexBaan.cxx
        ${LEXERS_DIR}/LexBash.cxx
        ${LEXERS_DIR}/LexBasic.cxx
        ${LEXERS_DIR}/LexBatch.cxx
        ${LEXERS_DIR}/LexBibTeX.cxx
        ${LEXERS_DIR}/LexBullant.cxx
        ${LEXERS_DIR}/LexCaml.cxx
        ${LEXERS_DIR}/LexCLW.cxx
        ${LEXERS_DIR}/LexCmake.cxx
        ${LEXERS_DIR}/LexCOBOL.cxx
        ${LEXERS_DIR}/LexCoffeeScript.cxx
        ${LEXERS_DIR}/LexConf.cxx
        ${LEXERS_DIR}/LexCPP.cxx
        ${LEXERS_DIR}/LexCrontab.cxx
        ${LEXERS_DIR}/LexCsound.cxx
        ${LEXERS_DIR}/LexCSS.cxx
        ${LEXERS_DIR}/LexD.cxx
        ${LEXERS_DIR}/LexDiff.cxx
        ${LEXERS_DIR}/LexDMAP.cxx
        ${LEXERS_DIR}/LexDMIS.cxx
        ${LEXERS_DIR}/LexECL.cxx
        ${LEXERS_DIR}/LexEDIFACT.cxx
        ${LEXERS_DIR}/LexEiffel.cxx
        ${LEXERS_DIR}/LexErlang.cxx
        ${LEXERS_DIR}/LexErrorList.cxx
        ${LEXERS_DIR}/LexEScript.cxx
        ${LEXERS_DIR}/LexFlagship.cxx
        ${LEXERS_DIR}/LexForth.cxx
        ${LEXERS_DIR}/LexFortran.cxx
        ${LEXERS_DIR}/LexGAP.cxx
        ${LEXERS_DIR}/LexGui4Cli.cxx
        ${LEXERS_DIR}/LexHaskell.cxx
        ${LEXERS_DIR}/LexHex.cxx
        ${LEXERS_DIR}/LexHTML.cxx
        ${LEXERS_DIR}/LexInno.cxx
        ${LEXERS_DIR}/LexJSON.cxx
        ${LEXERS_DIR}/LexKix.cxx
        ${LEXERS_DIR}/LexKVIrc.cxx
        ${LEXERS_DIR}/LexLaTeX.cxx
        ${LEXERS_DIR}/LexLisp.cxx
        ${LEXERS_DIR}/LexLout.cxx
        ${LEXERS_DIR}/LexLua.cxx
        ${LEXERS_DIR}/LexMagik.cxx
        ${LEXERS_DIR}/LexMake.cxx
        ${LEXERS_DIR}/LexMarkdown.cxx
        ${LEXERS_DIR}/LexMatlab.cxx
        ${LEXERS_DIR}/LexMetapost.cxx
        ${LEXERS_DIR}/LexMMIXAL.cxx
        ${LEXERS_DIR}/LexModula.cxx
        ${LEXERS_DIR}/LexMPT.cxx
        ${LEXERS_DIR}/LexMSSQL.cxx
        ${LEXERS_DIR}/LexMySQL.cxx
        ${LEXERS_DIR}/LexNimrod.cxx
        ${LEXERS_DIR}/LexNsis.cxx
        ${LEXERS_DIR}/LexNull.cxx
        ${LEXERS_DIR}/LexOpal.cxx
        ${LEXERS_DIR}/LexOScript.cxx
        ${LEXERS_DIR}/LexPascal.cxx
        ${LEXERS_DIR}/LexPB.cxx
        ${LEXERS_DIR}/LexPerl.cxx
        ${LEXERS_DIR}/LexPLM.cxx
        ${LEXERS_DIR}/LexPO.cxx
        ${LEXERS_DIR}/LexPOV.cxx
        ${LEXERS_DIR}/LexPowerPro.cxx
        ${LEXERS_DIR}/LexPowerShell.cxx
        ${LEXERS_DIR}/LexProgress.cxx
        ${LEXERS_DIR}/LexProps.cxx
        ${LEXERS_DIR}/LexPS.cxx
        ${LEXERS_DIR}/LexPython.cxx
        ${LEXERS_DIR}/LexR.cxx
        ${LEXERS_DIR}/LexRebol.cxx
        ${LEXERS_DIR}/LexRegistry.cxx
        ${LEXERS_DIR}/LexRuby.cxx
        ${LEXERS_DIR}/LexRust.cxx
        ${LEXERS_DIR}/LexScriptol.cxx
        ${LEXERS_DIR}/LexSmalltalk.cxx
        ${LEXERS_DIR}/LexSML.cxx
        ${LEXERS_DIR}/LexSorcus.cxx
        ${LEXERS_DIR}/LexSpecman.cxx
        ${LEXERS_DIR}/LexSpice.cxx
        ${LEXERS_DIR}/LexSQL.cxx
        ${LEXERS_DIR}/LexSTTXT.cxx
        ${LEXERS_DIR}/LexTACL.cxx
        ${LEXERS_DIR}/LexTADS3.cxx
        ${LEXERS_DIR}/LexTAL.cxx
        ${LEXERS_DIR}/LexTCL.cxx
        ${LEXERS_DIR}/LexTCMD.cxx
        ${LEXERS_DIR}/LexTeX.cxx
        ${LEXERS_DIR}/LexTxt2tags.cxx
        ${LEXERS_DIR}/LexVB.cxx
        ${LEXERS_DIR}/LexVerilog.cxx
        ${LEXERS_DIR}/LexVHDL.cxx
        ${LEXERS_DIR}/LexVisualProlog.cxx
        ${LEXERS_DIR}/LexYAML.cxx
    )
    target_link_libraries(lexer-examples PUBLIC lexlib)
    target_folder(lexer-examples "Libraries")
endif()
