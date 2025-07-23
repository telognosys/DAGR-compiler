#ifndef AST_NODES_H
#define AST_NODES_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

struct ASTNode {
    virtual ~ASTNode() = default;
};

struct ProgramNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
};

struct ReadNode : ASTNode {
    std::string file_pattern;
    std::string alias;
    std::vector<std::unique_ptr<ASTNode>> body;
};

struct AssignmentNode : ASTNode {
    std::string target;
    std::string source;
};

struct WhenNode : ASTNode {
    std::string variable;
    std::string pattern;
    std::vector<std::unique_ptr<ASTNode>> body;
};

struct EmitNode : ASTNode {
    std::unordered_map<std::string, std::string> emissions;
};

struct GroupNode : ASTNode {
    std::string group_var;
    std::vector<std::unique_ptr<ASTNode>> body;
};

struct CountNode : ASTNode {
    // vacio, sólo marca la operación count()
};

struct OutputNode : ASTNode {
    std::string filename;
};

#endif
