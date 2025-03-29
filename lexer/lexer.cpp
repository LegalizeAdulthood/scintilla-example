#include <ILexer.h>

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
    void Lex(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess) override;
    void Fold(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess) override;
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
    throw std::runtime_error("not implemented");
}

int Lexer::PropertyType(const char *name)
{
    throw std::runtime_error("not implemented");
}

const char *Lexer::DescribeProperty(const char *name)
{
    throw std::runtime_error("not implemented");
}

Sci_Position Lexer::PropertySet(const char *key, const char *val)
{
    throw std::runtime_error("not implemented");
}

const char *Lexer::DescribeWordListSets()
{
    throw std::runtime_error("not implemented");
}

Sci_Position Lexer::WordListSet(int n, const char *wl)
{
    throw std::runtime_error("not implemented");
}

void Lexer::Lex(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess)
{
    throw std::runtime_error("not implemented");
}

void Lexer::Fold(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess)
{
    throw std::runtime_error("not implemented");
}

void *Lexer::PrivateCall(int operation, void *pointer)
{
    throw std::runtime_error("not implemented");
}

ILexer *factory()
{
    return new Lexer;
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

extern "C" EXPORT int GetLexerCount()
{
    return 1;
}

extern "C" EXPORT void GetLexerName(unsigned int index, char *name, int size)
{
    if (index == 0)
    {
        std::strncpy(name, "id-formula", size);
    }
}

extern "C" EXPORT LexerFactoryFunction *GetLexerFactory(unsigned int index)
{
    if (index == 0)
    {
        return create_lexer;
    }
    return nullptr;
}
