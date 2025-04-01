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
    void none(StyleContext &sc);
    void comment(StyleContext &sc);
    void keyword(StyleContext &sc);

    WordList m_keywords;
    CharacterSet m_keyword_char_set{CharacterSet::setAlpha};
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

void Lexer::none(StyleContext &sc)
{
    if (sc.Match(';'))
    {
        sc.SetState(+formula::Syntax::COMMENT);
    }
    else if (m_keyword_char_set.Contains(sc.ch))
    {
        sc.SetState(+formula::Syntax::KEYWORD);
    }
    sc.Forward();
}

void Lexer::comment(StyleContext &sc)
{
    if (sc.Match('\n'))
    {
        sc.Forward(); // consume '\n' as part of the comment
        sc.SetState(+formula::Syntax::NONE);
    }
    else
    {
        sc.Forward();
    }
}

void Lexer::keyword(StyleContext &sc)
{
    if (sc.Match(';'))
    {
        sc.SetState(+formula::Syntax::COMMENT);
    }
    else if (!m_keyword_char_set.Contains(sc.ch))
    {
        sc.SetState(+formula::Syntax::NONE);
    }
    sc.Forward();
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
            if (sc.ch == ';' || !m_keyword_char_set.Contains(sc.ch))
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
            else if (m_keyword_char_set.Contains(sc.ch))
            {
                sc.SetState(+formula::Syntax::KEYWORD);
            }
        }

        if (advance)
        {
            sc.Forward();
        }
        continue;

        if (sc.state == +formula::Syntax::NONE)
        {
            none(sc);
        }
        else if (sc.state == +formula::Syntax::COMMENT)
        {
            comment(sc);
        }
        else if (sc.state == +formula::Syntax::KEYWORD)
        {
            keyword(sc);
        }
    }
    sc.Complete();
}

void Lexer::Fold(Sci_PositionU /*start*/, Sci_Position /*len*/, int /*init_style*/, IDocument */*doc*/)
{
}

void *Lexer::PrivateCall(int /*operation*/, void */*pointer*/)
{
    return nullptr;
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
