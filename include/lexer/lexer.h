#pragma once
#include "token.h"
#include <string>
#include <vector>

class Lexer {
public:
    explicit Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    char peek() const;
    char advance();
    bool isAtEnd() const;
    void skipWhitespace();
    void skipComment();

    Token makeIdentifier();
    Token makeString();
    Token makeSymbol();
    Token makeKeywordOrIdentifier();

    const std::string source;
    size_t current = 0;
    int line = 1;
    int column = 0;
};
