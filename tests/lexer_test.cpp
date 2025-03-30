#include <ILexer.h>

#include <wx/dynlib.h>
#include <wx/filename.h>
#include <wx/log.h>

#include <gtest/gtest.h>

#include <sstream>

class TestPlugin : public ::testing::Test
{
protected:
    void SetUp() override;
    wxFileName m_plugin_file{wxT("./formula-lexer") + wxDynamicLibrary::GetDllExt(wxDL_LIBRARY)};
    std::ostringstream m_log;
    wxLogStream m_logger{&m_log};
    wxDynamicLibrary plugin;
};

void TestPlugin::SetUp()
{
    wxLog::SetActiveTarget(&m_logger);
    plugin.Load(m_plugin_file.GetFullPath());
}

TEST_F(TestPlugin, pluginLoads)
{
    ASSERT_TRUE(plugin.IsLoaded()) << "full path: " << m_plugin_file.GetFullPath();
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

class TestPluginLoaded : public TestPlugin
{
protected:
    void SetUp() override
    {
        TestPlugin::SetUp();
        ASSERT_TRUE(plugin.IsLoaded());
    }
};

TEST_F(TestPluginLoaded, pluginExportsNecessaryFunctions)
{
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

TEST_F(TestPluginLoaded, oneLexerExported)
{
    using GetLexerCountFn = int();
    GetExportedSymbol get_lexer_count{plugin, wxT("GetLexerCount")};
    GetLexerCountFn *GetLexerCount{reinterpret_cast<GetLexerCountFn *>(get_lexer_count.function)};
    ASSERT_NE(nullptr, GetLexerCount);

    const int count = GetLexerCount();

    EXPECT_EQ(1, count);
}

TEST_F(TestPluginLoaded, lexerNameIsIdFormula)
{
    GetExportedSymbol get_lexer_name{plugin, wxT("GetLexerName")};
    using GetLexerNameFn = void(unsigned int index, char *buffer, int size);
    GetLexerNameFn *GetLexerName = reinterpret_cast<GetLexerNameFn *>(get_lexer_name.function);
    ASSERT_NE(nullptr, GetLexerName);

    char buffer[80]{};
    GetLexerName(0, buffer, sizeof(buffer));

    EXPECT_STREQ("id-formula", buffer);
}

TEST_F(TestPluginLoaded, factoryCreatesLexer)
{
    GetExportedSymbol get_lexer_factory{plugin, wxT("GetLexerFactory")};
    using LexerFactoryFunction = ILexer *();
    using GetLexerFactoryFn = LexerFactoryFunction *(unsigned int index);
    GetLexerFactoryFn *GetLexerFactory = reinterpret_cast<GetLexerFactoryFn *>(get_lexer_factory.function);
    ASSERT_NE(nullptr, GetLexerFactory);
    LexerFactoryFunction *factory{GetLexerFactory(0)};
    ASSERT_NE(nullptr, factory);

    ILexer *lexer = factory();

    ASSERT_NE(nullptr, lexer);

    lexer->Release();
}

class TestLexer : public TestPluginLoaded
{
protected:
    void SetUp() override;
    void TearDown() override;

    ILexer *m_lexer{};
};

void TestLexer::SetUp()
{
    TestPluginLoaded::SetUp();
    GetExportedSymbol get_lexer_factory{plugin, wxT("GetLexerFactory")};
    using LexerFactoryFunction = ILexer *();
    using GetLexerFactoryFn = LexerFactoryFunction *(unsigned int index);
    GetLexerFactoryFn *GetLexerFactory = reinterpret_cast<GetLexerFactoryFn *>(get_lexer_factory.function);
    ASSERT_NE(nullptr, GetLexerFactory);
    LexerFactoryFunction *factory{GetLexerFactory(0)};
    ASSERT_NE(nullptr, factory);
    m_lexer = factory();
    ASSERT_NE(nullptr, m_lexer);
}

void TestLexer::TearDown()
{
    if (m_lexer != nullptr)
    {
        m_lexer->Release();
    }
    TestPluginLoaded::TearDown();
}


TEST_F(TestLexer, version)
{
    int version{m_lexer->Version()};

    EXPECT_EQ(lvOriginal, version);
}
