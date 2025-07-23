#pragma once
#include <string>

enum class TokenType {
    Identifier,
    StringLiteral,
    Keyword,
    Symbol,
    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};
