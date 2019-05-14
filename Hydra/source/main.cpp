#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

#include "lang1.hpp"


int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("hydra: error: no input files\n");
        return EXIT_FAILURE;
    }

    std::string path(argv[1]);
    std::cout << path << std::endl;

    std::ifstream source;
    source.open(path);
    if (!source) {
        std::cout << "Error: cannot open file(" << path << ")" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Source file open: " << path << std::endl;

    std::string ast_name("main");
    Lexer lexer;
    std::vector<Token*> tokens;
    lexer.lex(source, tokens);
    //Parser parser;
    //AST ast(ast_name);
    //while (!source.eof()) {
    //    parser.parseNextExpression(source, ast);
    //}

    for (auto itr = tokens.begin(); itr != tokens.end(); itr++) {
        (*itr)->show();
    }
    for (auto itr = tokens.begin(); itr != tokens.end(); itr++) {
        delete *itr;
    }

    source.close();

    return EXIT_SUCCESS;
}
