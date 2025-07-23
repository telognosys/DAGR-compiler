#ifndef DAG_H
#define DAG_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

enum class DAGNodeType {
    Read,
    Assign,
    Filter,
    Emit,
    Group,
    Count,
    Output
};

struct DAGNode {
    DAGNodeType type;
    std::string name; // nombre o ID lógico del nodo
    std::unordered_map<std::string, std::string> metadata; // parámetros o info extra

    std::vector<DAGNode*> inputs;  // dependencias
    std::vector<DAGNode*> outputs; // nodos que dependen de este
};

struct DAG {
    std::vector<std::unique_ptr<DAGNode>> nodes;

    DAGNode* createNode(DAGNodeType type, const std::string& name);
    void connect(DAGNode* from, DAGNode* to);
};

#endif
