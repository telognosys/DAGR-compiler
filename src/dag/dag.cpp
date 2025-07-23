#include "dag/dag.h"

DAGNode* DAG::createNode(DAGNodeType type, const std::string& name) {
    auto node = std::make_unique<DAGNode>();
    node->type = type;
    node->name = name;
    nodes.push_back(std::move(node));
    return nodes.back().get();
}

void DAG::connect(DAGNode* from, DAGNode* to) {
    from->outputs.push_back(to);
    to->inputs.push_back(from);
}
