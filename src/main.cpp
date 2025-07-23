#include <iostream>
#include <fstream>
#include <sstream>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/ast_printer.h"
#include "dag/dag_builder.h"
#include "dag/dag_executor.h"

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: dagr_compiler <archivo.dagr>\n";
        return 1;
    }

    std::string source = readFile(argv[1]);

    // LEXER
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    // PARSER
    Parser parser(tokens);
    auto program = parser.parse();

    std::cout << "Parseo completado con éxito.\n";

    // Opcional: imprimir AST
    printAST(program.get());

    // DAG BUILDER
    DAGBuilder builder;
    auto dag = builder.build(program.get());

    std::cout << "Construcción del DAG completada.\n";

    // DAG EXECUTION
    DAGExecutor executor;
    executor.execute(*dag);

    std::cout << "Ejecución del DAG finalizada.\n";

    return 0;
}
