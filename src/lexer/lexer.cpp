#include "lexer/lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& src) : source(src) {}

char Lexer::peek() const {
    return current < source.size() ? source[current] : '\0';
}

char Lexer::advance() {
    if (current < source.size()) {
        column++;
        return source[current++];
    }
    return '\0';
}

bool Lexer::isAtEnd() const {
    return current >= source.size();
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else if (c == '\n') {
            advance();
            line++;
            column = 0;
        } else {
            break;
        }
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        skipWhitespace();

        char c = peek();
        if (std::isalpha(c) || c == '_') {
            tokens.push_back(makeKeywordOrIdentifier());
        } else if (c == '"') {
            tokens.push_back(makeString());
        } else if (c == '{' || c == '}' || c == ':' || c == '(' || c == ')' || c == '=' || c == '.' || c == '<' || c == '>') {
            tokens.push_back(makeSymbol());
            advance();
        } else {
            // Skip unknowns
            advance();
        }
    }

    tokens.push_back({TokenType::EndOfFile, "", line, column});
    return tokens;
}

Token Lexer::makeKeywordOrIdentifier() {
    size_t start = current;
    int startCol = column;

    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
        advance();
    }

    std::string text = source.substr(start, current - start);

    static const std::vector<std::string> keywords = {
        "read", "as", "when", "matches", "emit", "group", "emitted", "by", "count", "output", "to"
    };

    for (const auto& kw : keywords) {
        if (text == kw) {
            return {TokenType::Keyword, text, line, startCol};
        }
    }

    return {TokenType::Identifier, text, line, startCol};
}

Token Lexer::makeString() {
    int startCol = column;
    advance(); // skip opening "

    std::string value;
    while (!isAtEnd() && peek() != '"') {
        value += advance();
    }

    if (!isAtEnd()) advance(); // skip closing "

    return {TokenType::StringLiteral, value, line, startCol};
}

Token Lexer::makeSymbol() {
    char symbol = peek();
    std::string value(1, symbol);
    return {TokenType::Symbol, value, line, column};
}
