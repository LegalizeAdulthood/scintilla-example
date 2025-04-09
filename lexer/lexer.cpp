#include <formula/syntax.h>

#include <ILexer.h>
#include <Scintilla.h>

#include <assert.h>  // NOLINT(modernize-deprecated-headers); needed by LexAccessor.h
#include <stddef.h>  // NOLINT(modernize-deprecated-headers); needed by CharacterSet.h

#include <CharacterSet.h>
#include <LexAccessor.h>
#include <StyleContext.h>
#include <WordList.h>

#include <cctype>
#include <cstring>
#include <stdexcept>
#include <string>

namespace
{

class Lexer : public ILexer
{
public:
    Lexer();
    virtual ~Lexer() = default;

    int SCI_METHOD Version() const override;
    void SCI_METHOD Release() override;
    const char *SCI_METHOD PropertyNames() override;
    int SCI_METHOD PropertyType(const char *name) override;
    const char *SCI_METHOD DescribeProperty(const char *name) override;
    Sci_Position SCI_METHOD PropertySet(const char *key, const char *val) override;
    const char *SCI_METHOD DescribeWordListSets() override;
    Sci_Position SCI_METHOD WordListSet(int n, const char *wl) override;
    void SCI_METHOD Lex(Sci_PositionU start, Sci_Position len, int init_style, IDocument *doc) override;
    void SCI_METHOD Fold(Sci_PositionU start, Sci_Position len, int init_style, IDocument *doc) override;
    void *SCI_METHOD PrivateCall(int operation, void *pointer) override;

private:
    bool finish_state(StyleContext &sc);
    void begin_state(StyleContext &sc);

    WordList m_keywords;
    WordList m_functions;
    CharacterSet m_keyword_charset{CharacterSet::setAlpha};
    CharacterSet m_function_charset{CharacterSet::setAlphaNum};
    CharacterSet m_whitespace_charset{CharacterSet::setNone, " \t\v\f"};
    CharacterSet m_identifier_charset{CharacterSet::setAlphaNum};
    CharacterSet m_fold_keyword_charset{CharacterSet::setAlpha};
    bool m_maybe_keyword{};
    bool m_maybe_function{};
};

Lexer::Lexer()
{
    m_keywords.Set("if endif elseif else");
    m_functions.Set("sin cos sinh cosh cosxx tan cotan tanh cotanh sqr log exp "
                    "abs conj real imag flip fn1 fn2 fn3 fn4 srand asin asinh "
                    "acos acosh atan atanh sqrt cabs floor ceil trunc round");
}

int Lexer::Version() const
{
    return dvOriginal;
}

void Lexer::Release()
{
    delete this;
}

const char *Lexer::PropertyNames()
{
    return "";
}

int Lexer::PropertyType(const char */*name*/)
{
    return 0;
}

const char *Lexer::DescribeProperty(const char */*name*/)
{
    return "";
}

Sci_Position Lexer::PropertySet(const char */*key*/, const char */*val*/)
{
    return -1;
}

const char *Lexer::DescribeWordListSets()
{
    return "";
}

Sci_Position Lexer::WordListSet(int /*n*/, const char * /*wl*/)
{
    return -1;
}

void *Lexer::PrivateCall(int /*operation*/, void */*pointer*/)
{
    return nullptr;
}

bool Lexer::finish_state(StyleContext &sc)
{
    switch (sc.state)
    {
    case +formula::Syntax::COMMENT:
        if (sc.atLineStart)
        {
            sc.SetState(+formula::Syntax::NONE);
        }
        else if (sc.ch == '\n')
        {
            sc.Forward();
            sc.SetState(+formula::Syntax::NONE);
            return false;
        }
        break;

    case +formula::Syntax::KEYWORD:
        if (sc.ch == ';' || !m_keyword_charset.Contains(sc.ch))
        {
            sc.SetState(+formula::Syntax::NONE);
        }
        break;

    case +formula::Syntax::WHITESPACE:
        if (!m_whitespace_charset.Contains(sc.ch))
        {
            sc.SetState(+formula::Syntax::NONE);
        }
        break;

    case +formula::Syntax::FUNCTION:
        if (sc.ch == ';' || !m_function_charset.Contains(sc.ch))
        {
            sc.SetState(+formula::Syntax::NONE);
        }
        break;

    case +formula::Syntax::IDENTIFIER:
        if (sc.ch == ';' || !m_identifier_charset.Contains(sc.ch))
        {
            sc.SetState(+formula::Syntax::NONE);
        }
        break;

    default:
        break;
    }
    return true;
}

void Lexer::begin_state(StyleContext &sc)
{
    if (sc.state == +formula::Syntax::IDENTIFIER)
    {
        m_maybe_keyword = m_keyword_charset.Contains(sc.ch) && m_maybe_keyword;
        m_maybe_function = m_function_charset.Contains(sc.ch) && m_maybe_function;

        if (m_maybe_keyword)
        {
            char buffer[80];
            sc.GetCurrentLowered(buffer, sizeof(buffer));
            std::string current{buffer};
            current += static_cast<char>(std::tolower(static_cast<unsigned char>(sc.ch)));
            if (m_keywords.InList(current.c_str()) && !m_keyword_charset.Contains(sc.chNext))
            {
                sc.ChangeState(+formula::Syntax::KEYWORD);
                return;
            }
        }

        if (m_maybe_function)
        {
            char buffer[80];
            sc.GetCurrentLowered(buffer, sizeof(buffer));
            std::string current{buffer};
            current += static_cast<char>(std::tolower(sc.ch));
            if (m_functions.InList(current.c_str()) && !m_function_charset.Contains(sc.chNext))
            {
                sc.ChangeState(+formula::Syntax::FUNCTION);
                return;
            }
        }
    }
    if (sc.state != +formula::Syntax::NONE)
    {
        return;
    }

    if (sc.ch == ';')
    {
        sc.SetState(+formula::Syntax::COMMENT);
        return;
    }

    if (m_whitespace_charset.Contains(sc.ch))
    {
        sc.SetState(+formula::Syntax::WHITESPACE);
        return;
    }

    if (m_identifier_charset.Contains(sc.ch))
    {
        sc.SetState(+formula::Syntax::IDENTIFIER);
        m_maybe_keyword = m_keyword_charset.Contains(sc.ch);
        m_maybe_function = m_function_charset.Contains(sc.ch);
        return;
    }
}
void Lexer::Lex(Sci_PositionU start, Sci_Position len, int init_style, IDocument *doc)
{
    LexAccessor accessor{doc};
    StyleContext sc{start, static_cast<Sci_PositionU>(len), init_style, accessor};
    while (sc.More())
    {
        const bool advance{finish_state(sc)};
        begin_state(sc);

        if (advance)
        {
            sc.Forward();
        }
    }
    sc.Complete();
}

void Lexer::Fold(Sci_PositionU start, Sci_Position len, int init_style, IDocument *doc)
{
    LexAccessor accessor{doc};
    StyleContext sc{start, static_cast<Sci_PositionU>(len), init_style, accessor};
    int line = accessor.GetLine(start);
    int level = accessor.LevelAt(line);
    std::string text;
    bool in_keyword{false};
    bool skip_to_eol{false};
    while (sc.More())
    {
        if (sc.Match("\n"))
        {
            if (text == "if")
            {
                accessor.SetLevel(line, level | SC_FOLDLEVELHEADERFLAG);
                ++level;
            }
            else if (text == "elseif" || text == "else")
            {
                --level;
                accessor.SetLevel(line, level | SC_FOLDLEVELHEADERFLAG);
                ++level;
            }
            else if (text == "endif")
            {
                --level;
                accessor.SetLevel(line, level);
            }
            else
            {
                accessor.SetLevel(line, level);
            }
            text.clear();
            sc.Forward();
            ++line;
            skip_to_eol = false;
            in_keyword = false;
            continue;
        }

        if (in_keyword)
        {
            if (m_fold_keyword_charset.Contains(sc.ch))
            {
                text += static_cast<char>(std::tolower(static_cast<unsigned char>(sc.ch)));
            }
            else
            {
                in_keyword = false;
                skip_to_eol = true;
            }
        }
        else if (m_whitespace_charset.Contains(sc.ch))
        {
        }
        else if (!skip_to_eol)
        {
            if (m_fold_keyword_charset.Contains(sc.ch))
            {
                in_keyword = true;
                text += static_cast<char>(std::tolower(static_cast<unsigned char>(sc.ch)));
            }
        }
        sc.Forward();
    }
    if (!text.empty())
    {
        ++line;
        accessor.SetLevel(line, level);
    }
}

using LexerFactoryFunction = ILexer *();

ILexer *create_lexer()
{
    return new Lexer;
}


} // namespace

#if WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

extern "C" EXPORT int SCI_METHOD GetLexerCount()
{
    return 1;
}

extern "C" EXPORT void SCI_METHOD GetLexerName(unsigned int index, char *name, int size)
{
    if (index == 0)
    {
        std::strncpy(name, "id-formula", size);
    }
}

extern "C" EXPORT LexerFactoryFunction *SCI_METHOD GetLexerFactory(unsigned int index)
{
    if (index == 0)
    {
        return create_lexer;
    }
    return nullptr;
}
