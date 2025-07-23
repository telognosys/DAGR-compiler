#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include <string>
#include "lexer/token.h"
#include "ast/ast.h"

class Parser {
    const std::vector<Token>& tokens;
    size_t current = 0;

public:
    Parser(const std::vector<Token>& tokens);

    const Token& peek() const;
    const Token& advance();
    const Token& previous() const;

    bool isAtEnd() const;
    bool check(TokenType type, const std::string& value = "") const;
    bool match(TokenType type, const std::string& value = "");

    void error(const std::string& message);

    std::unique_ptr<ProgramNode> parse();

private:
    std::unique_ptr<ASTNode> parseStatement();

    std::unique_ptr<ReadNode> parseRead();
    std::unique_ptr<AssignmentNode> parseAssignment();
    std::unique_ptr<WhenNode> parseWhen();
    std::unique_ptr<EmitNode> parseEmit();
    std::unique_ptr<GroupNode> parseGroup();
    std::unique_ptr<CountNode> parseCount();
    std::unique_ptr<OutputNode> parseOutput();
};

#endif // PARSER_H
