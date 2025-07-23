#include "dag/runtime.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

// READ
Dataset executeRead(const DAGNode& node) {
    Dataset dataset;

    std::string pattern = node.metadata.at("pattern");
    std::string varName = node.name; // e.g., "read_logs"

    // Simulación: abre un solo archivo fijo por ahora
    std::ifstream file(pattern);
    if (!file.is_open()) {
        std::cerr << "Error: no se pudo abrir data/sample.log\n";
        return dataset;
    }

    std::string line;
    while (std::getline(file, line)) {
        Record record;
        record[varName] = line;
        dataset.push_back(record);
    }

    return dataset;
}

// FILTER (When)
Dataset executeFilter(const Dataset& input, const DAGNode& node) {
    Dataset output;
    std::string pattern = node.metadata.at("pattern");

    // Escapar caracteres especiales excepto '<' y '>'
    std::string escapedPattern;
    for (char c : pattern) {
        if (c == '.' || c == '+' || c == '?' || c == '*' || c == '^' || c == '$' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '\\' || c == '|') {
            escapedPattern += '\\';
        }
        escapedPattern += c;
    }

    // Reemplazar placeholders <var> por grupo capturador (.+?)
    std::string regexPattern = std::regex_replace(escapedPattern, std::regex(R"(<(.*?)>)"), "(.+?)");

    std::cout << "Regex generado: " << regexPattern << std::endl;

    std::regex re(regexPattern);

    for (const auto& record : input) {
        auto it = record.find("line");
        if (it == record.end()) {
            std::cerr << "No se encontró clave 'line' en el registro" << std::endl;
            continue;
        }

        std::smatch match;
        std::cout << "Evaluando: " << it->second << std::endl;
        if (std::regex_search(it->second, match, re)) {
            std::cout << "Coincidencia: " << it->second << std::endl;
            Record newRec = record;

            // Extraer nombres de variables
            std::regex varRegex("<(.*?)>");
            auto varIt = std::sregex_iterator(pattern.begin(), pattern.end(), varRegex);
            int group = 1;
            while (varIt != std::sregex_iterator()) {
                std::string varName = (*varIt)[1];
                if (group < match.size()) {
                    newRec[varName] = match[group++];
                }
                ++varIt;
            }
            output.push_back(newRec);
        }
    }

    return output;
}


// EMIT
Dataset executeEmit(const Dataset& input, const DAGNode& node) {
    Dataset output;

    for (const auto& rec : input) {
        Record outRec;
        for (const auto& [field, var] : node.metadata) {
            if (rec.count(var)) {
                outRec[field] = rec.at(var);
            }
        }
        output.push_back(outRec);
    }

    return output;
}

// GROUP
Dataset executeGroup(const Dataset& input, const DAGNode& node) {
    std::string key = node.metadata.at("key");
    std::unordered_map<std::string, Dataset> groups;

    for (const auto& rec : input) {
        std::string groupKey = rec.count(key) ? rec.at(key) : "<undefined>";
        groups[groupKey].push_back(rec);
    }

    // Cada grupo se representa como un registro con "group_key"
    Dataset output;
    for (const auto& [k, _] : groups) {
        output.push_back({{"group_key", k}});
    }

    return output;
}

// COUNT
Dataset executeCount(const Dataset& input, const DAGNode& node) {
    std::unordered_map<std::string, int> counts;

    for (const auto& rec : input) {
        std::string key = rec.count("group_key") ? rec.at("group_key") : "<undefined>";
        counts[key]++;
    }

    Dataset output;
    for (const auto& [k, count] : counts) {
        output.push_back({{"group_key", k}, {"count", std::to_string(count)}});
    }

    return output;
}

// OUTPUT
void executeOutput(const Dataset& input, const DAGNode& node) {
    std::string filename = node.metadata.at("file");
    std::ofstream out(filename);

    if (!out.is_open()) {
        std::cerr << "Error al escribir " << filename << "\n";
        return;
    }

    if (input.empty()) {
        out << "Empty result\n";
        return;
    }

    // Escribe encabezado
    for (const auto& [k, _] : input[0]) {
        out << k << ",";
    }
    out << "\n";

    for (const auto& rec : input) {
        for (const auto& [_, val] : rec) {
            out << val << ",";
        }
        out << "\n";
    }

    std::cout << "Archivo escrito: " << filename << "\n";
}

// ASSIGN
Dataset executeAssignment(const Dataset& input, const DAGNode& node) {
    Dataset output;

    std::string target = node.metadata.at("target");
    std::string source = node.metadata.at("expression");  // o "source" si así lo nombraste en el AST

    for (const auto& rec : input) {
        Record newRec = rec;
        if (rec.count(source)) {
            newRec[target] = rec.at(source);
        } else {
            std::cerr << "⚠️ Warning: campo '" << source << "' no encontrado en registro\n";
        }
        output.push_back(newRec);
    }

    return output;
}
