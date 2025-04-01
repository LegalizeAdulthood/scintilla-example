#include <formula/syntax.h>

#include <ILexer.h>

#include <wx/dynlib.h>
#include <wx/filename.h>
#include <wx/log.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

using namespace testing;

inline Sci_Position as_pos(size_t value)
{
    return static_cast<Sci_Position>(value);
}

MATCHER_P2(has_repeated_style_bytes, style_, size_, "")
{
    if (arg == nullptr)
    {
        *result_listener << "got nullptr for style bytes";
        return false;
    }

    bool result{true};
    for (size_t i = 0; i < size_; ++i)
    {
        std::uint8_t actual{static_cast<std::uint8_t>(arg[i])};
        if (actual != static_cast<std::uint8_t>(style_))
        {
            if (!result)
            {
                *result_listener << ", ";
            }
            *result_listener << "[" << i << "]: expected " << +style_ << ", got " << static_cast<int>(actual);
            result = false;
        }
    }

    if (result)
    {
        *result_listener << size_ << " style bytes match " << +style_;
    }
    return result;
}

MATCHER_P(has_style_bytes, expected, "")
{
    if (arg == nullptr)
    {
        *result_listener << "got nullptr for style bytes";
        return false;
    }

    bool result{true};
    for (size_t i = 0; i < expected.size(); ++i)
    {
        if (arg[i] != expected[i])
        {
            if (!result)
            {
                *result_listener << ", ";
            }
            *result_listener << "[" << i << "]: expected " << static_cast<int>(expected[i]) << ", got " << static_cast<int>(arg[i]);
            result = false;
        }
    }

    if (result)
    {
        *result_listener << expected.size() << " style bytes match";
    }
    return result;
}

class TestPlugin : public Test
{
protected:
    void SetUp() override;
    wxFileName m_plugin_file{wxT("./formula-lexer") + wxDynamicLibrary::GetDllExt(wxDL_LIBRARY)};
    std::ostringstream m_log;
    wxLogStream m_logger{&m_log};
    wxDynamicLibrary m_plugin;
};

void TestPlugin::SetUp()
{
    wxLog::SetActiveTarget(&m_logger);
    m_plugin.Load(m_plugin_file.GetFullPath());
}

TEST_F(TestPlugin, pluginLoads)
{
    ASSERT_TRUE(m_plugin.IsLoaded()) << "full path: " << m_plugin_file.GetFullPath();
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
        ASSERT_TRUE(m_plugin.IsLoaded());
    }
};

TEST_F(TestPluginLoaded, pluginExportsNecessaryFunctions)
{
    GetExportedSymbol get_lexer_count{m_plugin, wxT("GetLexerCount")};
    GetExportedSymbol get_lexer_name{m_plugin, wxT("GetLexerName")};
    GetExportedSymbol get_lexer_factory{m_plugin, wxT("GetLexerFactory")};
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
    GetExportedSymbol get_lexer_count{m_plugin, wxT("GetLexerCount")};
    GetLexerCountFn *GetLexerCount{reinterpret_cast<GetLexerCountFn *>(get_lexer_count.function)};
    ASSERT_NE(nullptr, GetLexerCount);

    const int count = GetLexerCount();

    EXPECT_EQ(1, count);
}

TEST_F(TestPluginLoaded, lexerNameIsIdFormula)
{
    GetExportedSymbol get_lexer_name{m_plugin, wxT("GetLexerName")};
    using GetLexerNameFn = void(unsigned int index, char *buffer, int size);
    GetLexerNameFn *GetLexerName = reinterpret_cast<GetLexerNameFn *>(get_lexer_name.function);
    ASSERT_NE(nullptr, GetLexerName);

    char buffer[80]{};
    GetLexerName(0, buffer, sizeof(buffer));

    EXPECT_STREQ("id-formula", buffer);
}

TEST_F(TestPluginLoaded, factoryCreatesLexer)
{
    GetExportedSymbol get_lexer_factory{m_plugin, wxT("GetLexerFactory")};
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
    GetExportedSymbol get_lexer_factory{m_plugin, wxT("GetLexerFactory")};
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

TEST_F(TestLexer, foldDoesNothing)
{
    m_lexer->Fold(0, 0, 0, nullptr);
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

class TestLexText : public TestLexer
{
protected:
    void SetUp() override;
    std::string m_text;
    MockDocument m_doc;
};

void TestLexText::SetUp()
{
    TestLexer::SetUp();
    EXPECT_CALL(m_doc, CodePage()).WillRepeatedly(Return(0));
    EXPECT_CALL(m_doc, Version()).WillRepeatedly(Return(dvOriginal));
    EXPECT_CALL(m_doc, StartStyling(0, _)).Times(1);
}

TEST_F(TestLexText, lexSemiColon)
{
    m_text = ";";
    EXPECT_CALL(m_doc, Length()).WillRepeatedly(Return(as_pos(m_text.size())));
    EXPECT_CALL(m_doc, LineFromPosition(0)).WillRepeatedly(Return(0));
    EXPECT_CALL(m_doc, LineFromPosition(1)).WillRepeatedly(Return(0));
    EXPECT_CALL(m_doc, LineStart(0)).WillRepeatedly(Return(0));
    EXPECT_CALL(m_doc, LineStart(Ge(1))).WillRepeatedly(Return(as_pos(m_text.size())));
    EXPECT_CALL(m_doc, GetCharRange(_, 0, as_pos(m_text.size())))
        .WillRepeatedly([&](char *dest, Sci_Position start, Sci_Position len)
            { std::strncpy(dest, m_text.substr(start, len).data(), len); });
    EXPECT_CALL(m_doc, SetStyles(as_pos(m_text.size()), has_repeated_style_bytes(formula::Syntax::COMMENT, m_text.size())))
        .WillOnce(Return(true));

    m_lexer->Lex(0, as_pos(m_text.size()), 0, &m_doc);
}

TEST_F(TestLexText, lexComment)
{
    m_text = "; this is a comment";
    EXPECT_CALL(m_doc, Length()).WillRepeatedly(Return(as_pos(m_text.size())));
    EXPECT_CALL(m_doc, LineFromPosition(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(m_doc, LineStart(0)).WillRepeatedly(Return(0));
    EXPECT_CALL(m_doc, LineStart(Ge(1))).WillRepeatedly(Return(as_pos(m_text.size())));
    EXPECT_CALL(m_doc, GetCharRange(_, 0, as_pos(m_text.size())))
        .WillRepeatedly([&](char *dest, Sci_Position start, Sci_Position len)
            { std::strncpy(dest, m_text.substr(start, len).data(), len); });
    EXPECT_CALL(m_doc, SetStyles(as_pos(m_text.size()), has_repeated_style_bytes(formula::Syntax::COMMENT, m_text.size())))
        .WillOnce(Return(true));

    m_lexer->Lex(0, as_pos(m_text.size()), 0, &m_doc);
}

TEST_F(TestLexText, lexMultipleCommentLines)
{
    m_text = "; first\n"
             "; second\n";
    std::vector<char> expected_styles;
    expected_styles.resize(m_text.size());
    std::fill(expected_styles.begin(), expected_styles.end(), static_cast<char>(+formula::Syntax::COMMENT));
    EXPECT_CALL(m_doc, Length()).WillRepeatedly(Return(as_pos(m_text.size())));
    EXPECT_CALL(m_doc, LineFromPosition(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(m_doc, LineStart(0)).WillRepeatedly(Return(0));
    EXPECT_CALL(m_doc, LineStart(Ge(1))).WillRepeatedly(Return(as_pos(m_text.size())));
    EXPECT_CALL(m_doc, GetCharRange(_, 0, as_pos(m_text.size())))
        .WillRepeatedly([&](char *dest, Sci_Position start, Sci_Position len)
            { std::strncpy(dest, m_text.substr(start, len).data(), len); });
    EXPECT_CALL(m_doc, SetStyles(as_pos(m_text.size()), has_style_bytes(expected_styles)))
        .WillOnce(Return(true));

    m_lexer->Lex(0, as_pos(m_text.size()), 0, &m_doc);
}

TEST_F(TestLexText, lexIfKeyword)
{
    m_text = "if";
    EXPECT_CALL(m_doc, Length()).WillRepeatedly(Return(as_pos(m_text.size())));
    EXPECT_CALL(m_doc, LineFromPosition(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(m_doc, LineStart(0)).WillRepeatedly(Return(0));
    EXPECT_CALL(m_doc, LineStart(Ge(1))).WillRepeatedly(Return(as_pos(m_text.size())));
    EXPECT_CALL(m_doc, GetCharRange(_, 0, as_pos(m_text.size())))
        .WillRepeatedly([&](char *dest, Sci_Position start, Sci_Position len)
            { std::strncpy(dest, m_text.substr(start, len).data(), len); });
    EXPECT_CALL(m_doc, SetStyles(as_pos(m_text.size()), has_repeated_style_bytes(formula::Syntax::KEYWORD, m_text.size())))
        .WillOnce(Return(true));

    m_lexer->Lex(0, as_pos(m_text.size()), 0, &m_doc);
}

TEST_F(TestLexText, lexCommentKeyword)
{
    const std::string comment{"; comment\n"};
    const std::string keyword{"if"};
    m_text = comment + keyword;
    EXPECT_CALL(m_doc, Length()).WillRepeatedly(Return(as_pos(m_text.size())));
    EXPECT_CALL(m_doc, LineFromPosition(0)).WillRepeatedly(Return(0));
    EXPECT_CALL(m_doc, LineStart(0)).WillRepeatedly(Return(0));
    EXPECT_CALL(m_doc, LineStart(1)).WillRepeatedly(Return(12));
    EXPECT_CALL(m_doc, LineStart(Ge(2))).WillRepeatedly(Return(-1));
    EXPECT_CALL(m_doc, LineFromPosition(12)).WillRepeatedly(Return(1));
    EXPECT_CALL(m_doc, GetCharRange(_, 0, as_pos(m_text.size())))
        .WillRepeatedly([&](char *dest, Sci_Position start, Sci_Position len)
            { std::strncpy(dest, m_text.substr(start, len).data(), len); });
    std::vector<char> styles;
    styles.resize(m_text.size());
    std::fill_n(styles.begin(), comment.size(), static_cast<char>(+formula::Syntax::COMMENT));
    std::fill_n(styles.begin() + comment.size(), keyword.size(), static_cast<char>(+formula::Syntax::KEYWORD));
    EXPECT_CALL(m_doc, SetStyles(as_pos(m_text.size()), has_style_bytes(styles)))
        .WillOnce(Return(true));

    m_lexer->Lex(0, as_pos(m_text.size()), 0, &m_doc);
}
