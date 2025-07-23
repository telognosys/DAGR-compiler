#ifndef DAG_RUNTIME_H
#define DAG_RUNTIME_H

#include "dag.h"
#include "data.h"

Dataset executeRead(const DAGNode& node);
Dataset executeFilter(const Dataset& input, const DAGNode& node);
Dataset executeEmit(const Dataset& input, const DAGNode& node);
Dataset executeGroup(const Dataset& input, const DAGNode& node);
Dataset executeCount(const Dataset& input, const DAGNode& node);
void    executeOutput(const Dataset& input, const DAGNode& node);

#endif // DAG_RUNTIME_H
