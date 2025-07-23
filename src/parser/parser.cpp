#include "parser/parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

bool Parser::check(TokenType type, const std::string& value) const {
    if (isAtEnd()) return false;
    const Token& token = peek();
    return token.type == type && (value.empty() || token.value == value);
}

bool Parser::match(TokenType type, const std::string& value) {
    if (check(type, value)) {
        advance();
        return true;
    }
    return false;
}

void Parser::error(const std::string& message) {
    std::cerr << "Parser error at line " << peek().line << ", col " << peek().column
              << ": " << message << "\n";
    std::exit(1);
}

// Entry point
std::unique_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();

    while (!isAtEnd()) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
    }

    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (match(TokenType::Keyword, "read")) return parseRead();
    if (match(TokenType::Keyword, "group")) return parseGroup();
    return nullptr;
}

// read "*.log" as log_file { ... }
std::unique_ptr<ReadNode> Parser::parseRead() {
    if (!match(TokenType::StringLiteral)) error("Expected string pattern after 'read'");
    std::string pattern = previous().value;

    if (!match(TokenType::Keyword, "as")) error("Expected 'as' after pattern");
    if (!match(TokenType::Identifier)) error("Expected identifier after 'as'");
    std::string alias = previous().value;

    if (!match(TokenType::Symbol, "{")) error("Expected '{' to start read block");

    auto node = std::make_unique<ReadNode>();
    node->file_pattern = pattern;
    node->alias = alias;

    while (!check(TokenType::Symbol, "}")) {
        if (isAtEnd()) error("Unterminated read block");

        if (check(TokenType::Identifier)) {
            node->body.push_back(parseAssignment());
        } else if (check(TokenType::Keyword, "when")) {
            advance(); // consume 'when'
            node->body.push_back(parseWhen());
        } else {
            error("Unexpected token inside read block");
        }
    }

    match(TokenType::Symbol, "}"); // consume '}'
    return node;
}

// line = log_file.readLine()
std::unique_ptr<AssignmentNode> Parser::parseAssignment() {
    if (!match(TokenType::Identifier)) error("Expected variable name for assignment");
    std::string target = previous().value;

    if (!match(TokenType::Symbol, "=")) error("Expected '=' in assignment");

    if (!match(TokenType::Identifier)) error("Expected source variable");
    std::string source = previous().value;

    if (!match(TokenType::Symbol, ".")) error("Expected '.' in method call");

    if (!match(TokenType::Identifier) || previous().value != "readLine")
        error("Expected 'readLine' method");

    if (!match(TokenType::Symbol, "(") || !match(TokenType::Symbol, ")"))
        error("Expected '()' after readLine");

    auto node = std::make_unique<AssignmentNode>();
    node->target = target;
    node->source = source;
    return node;
}

// when line matches "[...]" { emit { ... } }
std::unique_ptr<WhenNode> Parser::parseWhen() {
    if (!match(TokenType::Identifier)) error("Expected variable name in when");
    std::string var = previous().value;

    if (!match(TokenType::Keyword, "matches")) error("Expected 'matches'");

    if (!match(TokenType::StringLiteral)) error("Expected pattern string");
    std::string pattern = previous().value;

    if (!match(TokenType::Symbol, "{")) error("Expected '{' after when condition");

    auto node = std::make_unique<WhenNode>();
    node->variable = var;
    node->pattern = pattern;

    while (!check(TokenType::Symbol, "}")) {
        if (isAtEnd()) error("Unterminated when block");

        if (match(TokenType::Keyword, "emit")) {
            node->body.push_back(parseEmit());
        } else {
            error("Unexpected token inside when block");
        }
    }

    match(TokenType::Symbol, "}"); // consume '}'
    return node;
}

// emit { field: <var>, ... }
std::unique_ptr<EmitNode> Parser::parseEmit() {
    if (!match(TokenType::Symbol, "{")) error("Expected '{' after emit");

    auto node = std::make_unique<EmitNode>();

    while (!check(TokenType::Symbol, "}")) {
        if (!match(TokenType::Identifier)) error("Expected field name");
        std::string field = previous().value;

        if (!match(TokenType::Symbol, ":")) error("Expected ':'");

        if (!match(TokenType::Symbol, "<")) error("Expected '<'");
        if (!match(TokenType::Identifier)) error("Expected variable inside '<>'");
        std::string var = previous().value;
        if (!match(TokenType::Symbol, ">")) error("Expected '>'");

        node->emissions[field] = var;

        if (check(TokenType::Symbol, "}")) break;
    }

    match(TokenType::Symbol, "}"); // consume '}'
    return node;
}

// group emitted by <var> { count() output to "file" }
std::unique_ptr<GroupNode> Parser::parseGroup() {
    if (!match(TokenType::Keyword, "emitted")) error("Expected 'emitted'");
    if (!match(TokenType::Keyword, "by")) error("Expected 'by'");
    if (!match(TokenType::Identifier)) error("Expected group variable");
    std::string groupVar = previous().value;

    if (!match(TokenType::Symbol, "{")) error("Expected '{' to start group block");

    auto node = std::make_unique<GroupNode>();
    node->group_var = groupVar;

    while (!check(TokenType::Symbol, "}")) {
        if (check(TokenType::Keyword, "count")) {
            node->body.push_back(parseCount());
        } else if (check(TokenType::Keyword, "output")) {
            node->body.push_back(parseOutput());
        } else {
            error("Unexpected token in group block");
        }
    }

    match(TokenType::Symbol, "}");
    return node;
}

std::unique_ptr<CountNode> Parser::parseCount() {
    match(TokenType::Keyword, "count");
    if (!match(TokenType::Symbol, "(") || !match(TokenType::Symbol, ")"))
        error("Expected count()");
    return std::make_unique<CountNode>();
}

std::unique_ptr<OutputNode> Parser::parseOutput() {
    match(TokenType::Keyword, "output");
    if (!match(TokenType::Keyword, "to")) error("Expected 'to'");
    if (!match(TokenType::StringLiteral)) error("Expected output filename");

    auto node = std::make_unique<OutputNode>();
    node->filename = previous().value;
    return node;
}
