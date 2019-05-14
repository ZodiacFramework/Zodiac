#pragma once

#include <stdint.h>

#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

typedef uint32_t type_t;

enum LANG1_TYPES {
    LANG1_PROGRAM = 0,
    LANG1_BLOCK = 1,
    LANG1_ATOM = 2,
};

class Field {

};

class Block {
    private:
        std::string content;
        std::map<std::string, int> states;
    public:
        Block();
        ~Block();
};

class Program {

};

class Context {

};

class NameSpace {

};

class Object {
    private:
        type_t type;
        void *ptr;
    private:
        Object(type_t type);
        ~Object();
};

class ASTNode {
    private:
        type_t token_type;
        std::string token;
        std::vector<ASTNode *> leaf;
        void *p;
    public:
        ASTNode(type_t t, std::string &atom);
        ~ASTNode();
        void asBlock(std::string &block_name);
};

class AST {
    private:
        ASTNode *root;
        ASTNode *cursol;
    public:
        AST(std::string &str);
        ~AST();
        void appendLeaf(type_t t);
        void moveCursol(size_t idx);
};

enum LEX_TOKEN_STATE {
    LEX_LINEFEED = 0,
    LEX_INDENT = 1,
};

class Token {
    private:
        bool lexing_block;
        uint64_t type;
        uint64_t status_flag;
        size_t line;
        size_t column;
        std::string token;
    public:
        Token();
        Token(type_t t);
        ~Token();
        void setType(uint64_t t);
        uint64_t getType();
        void setFlag(uint64_t flag);
        void setPosition(size_t l, size_t c);
        void show();
        void push(char c);
};

enum LEX_STATE {
    LEX_NORMAL = 0,
    LEX_BLOCK = 1,
};

class Lexer {
    private:
        size_t line;
        size_t column;
        //uint64_t lex_state;
        Token *cur;
        bool eof;
        std::ifstream *source;
    public:
        Lexer();
        ~Lexer();
        void lex(std::ifstream *source, std::vector<Token*> &tokens);
        void separateWith(std::vector<Token*> &tokens, char c);
        void lexChar(std::vector<Token*> &tokens, char c);
        void lexBlock(std::vector<Token*> &tokens);
        void newLine(std::vector<Token*> &tokens);
};

class Parser {
    public:
        Parser();
        ~Parser();
        void parseNextExpression(std::ifstream &source, AST &ast);
};
