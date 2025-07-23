#include "dag/dag_executor.h"
#include <iostream>

// Fwd declarations (luego van en un runtime separado)
Dataset executeRead(const DAGNode& node);
Dataset executeFilter(const Dataset& input, const DAGNode& node);
Dataset executeEmit(const Dataset& input, const DAGNode& node);
Dataset executeGroup(const Dataset& input, const DAGNode& node);
Dataset executeCount(const Dataset& input, const DAGNode& node);
void    executeOutput(const Dataset& input, const DAGNode& node);

void DAGExecutor::execute(const DAG& dag) {
    for (const auto& nodePtr : dag.nodes) {
        executeNode(nodePtr.get());
    }
}

void DAGExecutor::executeNode(const DAGNode* node) {
    Dataset input;

    // Recoge y fusiona outputs de nodos anteriores
    for (const auto& dep : node->inputs) {
        const auto& depOutput = results[dep];
        input.insert(input.end(), depOutput.begin(), depOutput.end());
    }

    Dataset output;

    switch (node->type) {
        case DAGNodeType::Read:
            output = executeRead(*node);
            break;
        case DAGNodeType::Filter:
            output = executeFilter(input, *node);
            break;
        case DAGNodeType::Emit:
            output = executeEmit(input, *node);
            break;
        case DAGNodeType::Group:
            output = executeGroup(input, *node);
            break;
        case DAGNodeType::Count:
            output = executeCount(input, *node);
            break;
        case DAGNodeType::Output:
            executeOutput(input, *node);
            output = input; // for consistency
            break;
        case DAGNodeType::Assign:
            output = executeAssignment(input, *node);
            break;
    }

    results[node] = output;
}
