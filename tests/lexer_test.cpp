#include <ILexer.h>

#include <wx/dynlib.h>
#include <wx/filename.h>
#include <wx/log.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstring>
#include <sstream>

using namespace testing;

class TestPlugin : public Test
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

TEST_F(TestLexer, noPropertyNames)
{
    EXPECT_STREQ("", m_lexer->PropertyNames());
}

TEST_F(TestLexer, noPropertyType)
{
    EXPECT_EQ(0, m_lexer->PropertyType(nullptr));
}

TEST_F(TestLexer, noPropertyDescription)
{
    EXPECT_STREQ("", m_lexer->DescribeProperty(nullptr));
}

static constexpr int NO_LEXING_REQUIRED{-1};

TEST_F(TestLexer, propertySetRequiresNoLexing)
{
    EXPECT_EQ(NO_LEXING_REQUIRED, m_lexer->PropertySet(nullptr, nullptr));
}

TEST_F(TestLexer, noWordListSetsDescription)
{
    EXPECT_STREQ("", m_lexer->DescribeWordListSets());
}

TEST_F(TestLexer, wordListSetRequiresNoLexing)
{
    EXPECT_EQ(NO_LEXING_REQUIRED, m_lexer->WordListSet(0, nullptr));
}

TEST_F(TestLexer, privateCallReturnsNullPtr)
{
    EXPECT_EQ(nullptr, m_lexer->PrivateCall(0, nullptr));
}

class MockDocument : public StrictMock<IDocument>
{
public:
    virtual ~MockDocument() = default;
    MOCK_METHOD(int, Version, (), (const, override));
    MOCK_METHOD(void, SetErrorStatus, (int), (override));
    MOCK_METHOD(Sci_Position, Length, (), (const, override));
    MOCK_METHOD(void, GetCharRange, (char *, Sci_Position, Sci_Position), (const, override));
    MOCK_METHOD(char, StyleAt, (Sci_Position), (const, override));
    MOCK_METHOD(Sci_Position, LineFromPosition, (Sci_Position), (const, override));
    MOCK_METHOD(Sci_Position, LineStart, (Sci_Position), (const, override));
    MOCK_METHOD(int, GetLevel, (Sci_Position), (const, override));
    MOCK_METHOD(int, SetLevel, (Sci_Position, int), (override));
    MOCK_METHOD(int, GetLineState, (Sci_Position), (const, override));
    MOCK_METHOD(int, SetLineState, (Sci_Position, int), (override));
    MOCK_METHOD(void, StartStyling, (Sci_Position, char), (override));
    MOCK_METHOD(bool, SetStyleFor, (Sci_Position, char), (override));
    MOCK_METHOD(bool, SetStyles, (Sci_Position, const char *), (override));
    MOCK_METHOD(void, DecorationSetCurrentIndicator, (int), (override));
    MOCK_METHOD(void, DecorationFillRange, (Sci_Position, int, Sci_Position), (override));
    MOCK_METHOD(void, ChangeLexerState, (Sci_Position, Sci_Position), (override));
    MOCK_METHOD(int, CodePage, (), (const, override));
    MOCK_METHOD(bool, IsDBCSLeadByte, (char), (const, override));
    MOCK_METHOD(const char *, BufferPointer, (), (override));
    MOCK_METHOD(int, GetLineIndentation, (Sci_Position), (override));
};

TEST_F(TestLexer, lexComment)
{
    std::string_view text{";"};
    MockDocument doc;
    EXPECT_CALL(doc, CodePage()).WillRepeatedly(Return(0));
    EXPECT_CALL(doc, Length()).WillRepeatedly(Return(static_cast<Sci_Position>(text.size())));
    EXPECT_CALL(doc, Version()).WillRepeatedly(Return(dvOriginal));
    EXPECT_CALL(doc, StartStyling(0, _)).Times(1);
    EXPECT_CALL(doc, LineFromPosition(0)).WillRepeatedly(Return(0));
    EXPECT_CALL(doc, LineFromPosition(1)).WillRepeatedly(Return(0));
    EXPECT_CALL(doc, LineStart(0)).WillRepeatedly(Return(0));
    EXPECT_CALL(doc, LineStart(Ge(1))).WillRepeatedly(Return(static_cast<Sci_Position>(text.size())));
    EXPECT_CALL(doc, GetCharRange(_, 0, 1))
        .WillRepeatedly([&](char *dest, Sci_Position start, Sci_Position len)
            { std::strncpy(dest, text.substr(start, len).data(), len); });

    m_lexer->Lex(0, static_cast<Sci_Position>(text.size()), 0, &doc);
}
