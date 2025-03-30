#include "config.h"

#include <ILexer.h>

#include <wx/dynlib.h>
#include <wx/filename.h>
#include <wx/log.h>

#include <gtest/gtest.h>

#include <sstream>

class TestLexer : public ::testing::Test
{
protected:
    void SetUp() override;

    wxFileName m_plugin_file{wxT("./formula-lexer") + wxDynamicLibrary::GetDllExt(wxDL_LIBRARY)};
    std::shared_ptr<ILexer> lexer{};
};

void TestLexer::SetUp()
{
    m_plugin_file.DirName(PLUGIN_DIR);
}

TEST_F(TestLexer, pluginLoads)
{
    std::ostringstream log;
    wxLogStream logger(&log);
    wxLog::SetActiveTarget(&logger);
    wxDynamicLibrary plugin(m_plugin_file.GetFullPath());

    ASSERT_TRUE(plugin.IsLoaded()) << "PLUGIN_DIR: " << PLUGIN_DIR << ", full path: " << m_plugin_file.GetFullPath();
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
    wxDynamicLibrary plugin(m_plugin_file.GetFullPath());
    ASSERT_TRUE(plugin.IsLoaded());

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

TEST_F(TestLexer, oneLexerExported)
{
    std::ostringstream log;
    wxLogStream logger(&log);
    wxLog::SetActiveTarget(&logger);
    wxDynamicLibrary plugin(m_plugin_file.GetFullPath());
    ASSERT_TRUE(plugin.IsLoaded());
    using GetLexerCountFn = int();
    GetExportedSymbol get_lexer_count{plugin, wxT("GetLexerCount")};
    GetLexerCountFn *GetLexerCount{reinterpret_cast<GetLexerCountFn*>(get_lexer_count.function)};
    ASSERT_NE(nullptr, GetLexerCount);

    const int count = GetLexerCount();

    EXPECT_EQ(1, count);
}

TEST_F(TestLexer, lexerNameIsIdFormula)
{
    std::ostringstream log;
    wxLogStream logger(&log);
    wxLog::SetActiveTarget(&logger);
    wxDynamicLibrary plugin(m_plugin_file.GetFullPath());
    ASSERT_TRUE(plugin.IsLoaded());
    GetExportedSymbol get_lexer_name{plugin, wxT("GetLexerName")};
    using GetLexerNameFn = void(unsigned int index, char *buffer, int size);
    GetLexerNameFn *GetLexerName = reinterpret_cast<GetLexerNameFn*>(get_lexer_name.function);
    ASSERT_NE(nullptr, GetLexerName);

    char buffer[80]{};
    GetLexerName(0, buffer, sizeof(buffer));

    EXPECT_STREQ("id-formula", buffer);
}

TEST_F(TestLexer, factoryCreatesLexer)
{
    std::ostringstream log;
    wxLogStream logger(&log);
    wxLog::SetActiveTarget(&logger);
    wxDynamicLibrary plugin(m_plugin_file.GetFullPath());
    ASSERT_TRUE(plugin.IsLoaded());
    GetExportedSymbol get_lexer_factory{plugin, wxT("GetLexerFactory")};
    using LexerFactoryFunction = ILexer *();
    using GetLexerFactoryFn = LexerFactoryFunction *(unsigned int index);
    GetLexerFactoryFn *GetLexerFactory = reinterpret_cast<GetLexerFactoryFn *>(get_lexer_factory.function);
    ASSERT_NE(nullptr, GetLexerFactory);
    LexerFactoryFunction *factory{GetLexerFactory(0)};
    ASSERT_NE(nullptr, factory);

    char buffer[80]{};
    ILexer *lexer = factory();

    ASSERT_NE(nullptr, lexer);

    lexer->Release();
}
