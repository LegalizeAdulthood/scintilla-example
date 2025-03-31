#include <ILexer.h>

#include <assert.h>  // NOLINT(modernize-deprecated-headers); needed by LexAccessor.h
#include <LexAccessor.h>
#include <StyleContext.h>

#include <cstring>
#include <stdexcept>

namespace
{

class Lexer : public ILexer
{
public:
    virtual ~Lexer() = default;

    int Version() const override;
    void Release() override;
    const char *PropertyNames() override;
    int PropertyType(const char *name) override;
    const char *DescribeProperty(const char *name) override;
    Sci_Position PropertySet(const char *key, const char *val) override;
    const char *DescribeWordListSets() override;
    Sci_Position WordListSet(int n, const char *wl) override;
    void Lex(Sci_PositionU start, Sci_Position len, int init_style, IDocument *doc) override;
    void Fold(Sci_PositionU start, Sci_Position len, int init_style, IDocument *doc) override;
    void *PrivateCall(int operation, void *pointer) override;
};

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

Sci_Position Lexer::WordListSet(int /*n*/, const char */*wl*/)
{
    return -1;
}

enum
{
    FRM_DEFAULT = 0,
    FRM_COMMENT = 1,
};

void Lexer::Lex(Sci_PositionU start, Sci_Position len, int init_style, IDocument *doc)
{
    LexAccessor accessor{doc};
    StyleContext context{start, static_cast<Sci_PositionU>(len), init_style, accessor};
    for (; context.More(); context.Forward())
    {
        if (context.state == FRM_DEFAULT)
        {
            if (context.Match(';'))
            {
                context.SetState(FRM_COMMENT);
            }
        }
        else if (context.state == FRM_COMMENT)
        {
            if (context.Match('\n'))
            {
                context.SetState(FRM_DEFAULT);
            }
        }
    }
    context.Complete();
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
