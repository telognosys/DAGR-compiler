#ifndef DAG_EXECUTOR_H
#define DAG_EXECUTOR_H

#include "dag.h"
#include "data.h"

#include <unordered_map>
#include <memory>

class DAGExecutor {
public:
    void execute(const DAG& dag);

private:
    std::unordered_map<const DAGNode*, Dataset> results;

    void executeNode(const DAGNode* node);
};

Dataset executeAssignment(const Dataset& input, const DAGNode& node);

#endif // DAG_EXECUTOR_H
