#ifndef DAG_BUILDER_H
#define DAG_BUILDER_H

#include "dag.h"
#include "ast/ast.h"

class DAGBuilder {
public:
    DAGBuilder();

    std::unique_ptr<DAG> build(const ProgramNode* ast);

private:
    DAG* dag;
    DAGNode* lastNode = nullptr;

    void visit(const ASTNode* node);
    void visitRead(const ReadNode* node);
    void visitAssignment(const AssignmentNode* node);
    void visitWhen(const WhenNode* node);
    void visitEmit(const EmitNode* node);
    void visitGroup(const GroupNode* node);
    void visitCount(const CountNode* node);
    void visitOutput(const OutputNode* node);
};

#endif
