#ifndef DAG_DATA_H
#define DAG_DATA_H

#include <string>
#include <unordered_map>
#include <vector>

using Record = std::unordered_map<std::string, std::string>;
using Dataset = std::vector<Record>;

#endif // DAG_DATA_H
