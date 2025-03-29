#include <ILexer.h>

#include <wx/dynlib.h>
#include <wx/log.h>

#include <gtest/gtest.h>

#include <sstream>

class TestLexer : public ::testing::Test
{
protected:
    std::shared_ptr<ILexer> lexer{};
};

TEST_F(TestLexer, pluginLoads)
{
    wxDynamicLibrary plugin(wxT("formula-lexer"));

    ASSERT_TRUE(plugin.IsLoaded());
}

struct GetExportedSymbol
{
    GetExportedSymbol(const wxDynamicLibrary &plugin, const wxString &name)
    {
        function = plugin.GetSymbol(name, &found);
    }
    void *function{};
    bool found{};
};

TEST_F(TestLexer, pluginExportsNecessaryFunctions)
{
    std::ostringstream log;
    wxLogStream logger(&log);
    wxLog::SetActiveTarget(&logger);
    wxDynamicLibrary plugin(wxT("formula-lexer"));

    GetExportedSymbol get_lexer_count{plugin, wxT("GetLexerCount")};
    GetExportedSymbol get_lexer_name{plugin, wxT("GetLexerName")};
    GetExportedSymbol get_lexer_factory{plugin, wxT("GetLexerFactory")};
    EXPECT_TRUE(get_lexer_count.found);
    EXPECT_NE(nullptr, get_lexer_count.function);
    EXPECT_TRUE(get_lexer_name.found);
    EXPECT_NE(nullptr, get_lexer_name.function);
    EXPECT_TRUE(get_lexer_factory.found);
    EXPECT_NE(nullptr, get_lexer_factory.function);
}
