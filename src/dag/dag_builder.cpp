#include "dag/dag_builder.h"

DAGBuilder::DAGBuilder() {}

std::unique_ptr<DAG> DAGBuilder::build(const ProgramNode* ast) {
    dag = new DAG();

    for (const auto& stmt : ast->statements) {
        visit(stmt.get());
    }

    return std::unique_ptr<DAG>(dag);
}

void DAGBuilder::visit(const ASTNode* node) {
    if (auto n = dynamic_cast<const ReadNode*>(node)) visitRead(n);
    else if (auto n = dynamic_cast<const AssignmentNode*>(node)) visitAssignment(n);
    else if (auto n = dynamic_cast<const WhenNode*>(node)) visitWhen(n);
    else if (auto n = dynamic_cast<const EmitNode*>(node)) visitEmit(n);
    else if (auto n = dynamic_cast<const GroupNode*>(node)) visitGroup(n);
    else if (auto n = dynamic_cast<const CountNode*>(node)) visitCount(n);
    else if (auto n = dynamic_cast<const OutputNode*>(node)) visitOutput(n);
}

void DAGBuilder::visitAssignment(const AssignmentNode* node) {
    auto assignNode = dag->createNode(DAGNodeType::Assign, "assign_" + node->target);
    assignNode->metadata["target"] = node->target;
    assignNode->metadata["expression"] = node->source; // <-- Cambiado

    if (lastNode) {
        dag->connect(lastNode, assignNode);
    }

    lastNode = assignNode;
}

void DAGBuilder::visitWhen(const WhenNode* node) {
    auto filterNode = dag->createNode(DAGNodeType::Filter, "when_" + node->pattern); // <-- Cambiado
    filterNode->metadata["pattern"] = node->pattern;                                // <-- Cambiado

    if (lastNode) {
        dag->connect(lastNode, filterNode);
    }

    lastNode = filterNode;

    for (const auto& stmt : node->body) {
        visit(stmt.get());
    }
}

void DAGBuilder::visitEmit(const EmitNode* node) {
    auto emitNode = dag->createNode(DAGNodeType::Emit, "emit");
    for (const auto& [field, var] : node->emissions) {
        emitNode->metadata[field] = var;
    }

    if (lastNode) {
        dag->connect(lastNode, emitNode);
    }

    lastNode = emitNode;
}

void DAGBuilder::visitGroup(const GroupNode* node) {
    auto groupNode = dag->createNode(DAGNodeType::Group, "group_by_" + node->group_var); // <-- Cambiado
    groupNode->metadata["key"] = node->group_var;                                        // <-- Cambiado

    if (lastNode) {
        dag->connect(lastNode, groupNode);
    }

    DAGNode* parent = lastNode;
    lastNode = groupNode;

    for (const auto& stmt : node->body) {
        visit(stmt.get());
    }

    lastNode = parent;
}


void DAGBuilder::visitCount(const CountNode* node) {
    auto countNode = dag->createNode(DAGNodeType::Count, "count");

    if (lastNode) {
        dag->connect(lastNode, countNode);
    }

    lastNode = countNode;
}

void DAGBuilder::visitOutput(const OutputNode* node) {
    auto outputNode = dag->createNode(DAGNodeType::Output, "output");
    outputNode->metadata["file"] = node->filename;

    if (lastNode) {
        dag->connect(lastNode, outputNode);
    }

    lastNode = outputNode;
}

void DAGBuilder::visitRead(const ReadNode* node) {
    auto readNode = dag->createNode(DAGNodeType::Read, "read_" + node->alias);
    readNode->metadata["pattern"] = node->file_pattern;
    readNode->name = node->alias;

    if (lastNode) {
        dag->connect(lastNode, readNode);
    }

    lastNode = readNode;

    for (const auto& stmt : node->body) {
        visit(stmt.get());
    }
}
