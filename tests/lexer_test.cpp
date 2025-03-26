#include <lexer/lexer.h>

#include <gtest/gtest.h>

class TestLexer : public ::testing::Test
{
protected:
    std::shared_ptr<ILexer> lexer{lexer::createLexer(), [](ILexer *lexer) { lexer->Release(); }};
};

TEST_F(TestLexer, create)
{
    ASSERT_TRUE(lexer);
}

TEST_F(TestLexer, version)
{
    ASSERT_EQ(dvOriginal, lexer->Version());
}
