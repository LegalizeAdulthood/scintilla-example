#include <formula/syntax.h>

#include <ILexer.h>

#include <assert.h>  // NOLINT(modernize-deprecated-headers); needed by LexAccessor.h

#include <CharacterSet.h>
#include <LexAccessor.h>
#include <StyleContext.h>
#include <WordList.h>

#include <cstring>
#include <stdexcept>

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
    WordList m_keywords;
    CharacterSet m_keyword_charset{CharacterSet::setAlpha};
    CharacterSet m_whitespace_charset{CharacterSet::setNone, " \t\v\f"};
};

Lexer::Lexer()
{
    m_keywords.Set("if");
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

void Lexer::Fold(Sci_PositionU /*start*/, Sci_Position /*len*/, int /*init_style*/, IDocument */*doc*/)
{
}

void *Lexer::PrivateCall(int /*operation*/, void */*pointer*/)
{
    return nullptr;
}

void Lexer::Lex(Sci_PositionU start, Sci_Position len, int init_style, IDocument *doc)
{
    LexAccessor accessor{doc};
    StyleContext sc{start, static_cast<Sci_PositionU>(len), init_style, accessor};
    while (sc.More())
    {
        bool advance{true};
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
                advance = false;
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

        default:
            break;
        }

        if (sc.state == +formula::Syntax::NONE)
        {
            if (sc.ch == ';')
            {
                sc.SetState(+formula::Syntax::COMMENT);
            }
            else if (m_whitespace_charset.Contains(sc.ch))
            {
                sc.SetState(+formula::Syntax::WHITESPACE);
            }
            else if (m_keyword_charset.Contains(sc.ch))
            {
                char buffer[80];
                sc.GetCurrent(buffer, sizeof(buffer));
                std::string current{buffer};
                current += static_cast<char>(sc.ch);
                if (m_keywords.InList(current.c_str()))
                {
                    sc.ChangeState(+formula::Syntax::KEYWORD);
                }
            }
        }

        if (advance)
        {
            sc.Forward();
        }
    }
    sc.Complete();
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
