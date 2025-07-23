#include "ast/ast.h"
#include <iostream>
#include <string>

void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
}

void printAST(const ASTNode* node, int indent = 0) {
    if (!node) return;

    if (auto program = dynamic_cast<const ProgramNode*>(node)) {
        printIndent(indent); std::cout << "Program\n";
        for (const auto& stmt : program->statements) {
            printAST(stmt.get(), indent + 1);
        }
    }
    else if (auto read = dynamic_cast<const ReadNode*>(node)) {
        printIndent(indent); 
        std::cout << "Read: pattern=\"" << read->file_pattern << "\" as " << read->alias << "\n";
        for (const auto& stmt : read->body) {
            printAST(stmt.get(), indent + 1);
        }
    }
    else if (auto assign = dynamic_cast<const AssignmentNode*>(node)) {
        printIndent(indent); 
        std::cout << "Assignment: " << assign->target << " = " << assign->source << "\n";
    }
    else if (auto when = dynamic_cast<const WhenNode*>(node)) {
        printIndent(indent); 
        std::cout << "When: matches \"" << when->pattern << "\"\n";
        for (const auto& stmt : when->body) {
            printAST(stmt.get(), indent + 1);
        }
    }
    else if (auto emit = dynamic_cast<const EmitNode*>(node)) {
        printIndent(indent); std::cout << "Emit:\n";
        for (const auto& [field, var] : emit->emissions) {
            printIndent(indent + 1); std::cout << field << ": <" << var << ">\n";
        }
    }
    else if (auto group = dynamic_cast<const GroupNode*>(node)) {
        printIndent(indent); 
        std::cout << "Group by " << group->group_var << "\n";
        for (const auto& stmt : group->body) {
            printAST(stmt.get(), indent + 1);
        }
    }
    else if (dynamic_cast<const CountNode*>(node)) {
        printIndent(indent); std::cout << "Count()\n";
    }
    else if (auto out = dynamic_cast<const OutputNode*>(node)) {
        printIndent(indent); std::cout << "Output to \"" << out->filename << "\"\n";
    }
    else {
        printIndent(indent); std::cout << "Unknown AST node\n";
    }
}
