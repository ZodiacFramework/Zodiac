#include "lang1.hpp"

Block::Block() {}
Block::~Block() {}

Object::Object(type_t t) :
    type(t)
{
    switch (type) {
        case LANG1_BLOCK:
            ptr = (void *)(new Block());
    }
}

Object::~Object() {
    switch (type) {
        case LANG1_BLOCK:
            delete (Block *)ptr;
    }
}

ASTNode::ASTNode(type_t t, std::string &str) :
    token_type(t), token(str), p(nullptr) {}

ASTNode::~ASTNode() {}

void ASTNode::asBlock(std::string &block_name) {

}

AST::AST(std::string &str) {
    root = new ASTNode(LANG1_PROGRAM, str);
    cursol = root;
}

AST::~AST() {
    delete root;
}

enum TOKEN_TYPE {
    TOKEN_EOF = 0,          // End of File
    TOKEN_NOTYPE,           // The first character is read to Token
    TOKEN_LINEFEED,         // Linefeed('\n')
    TOKEN_INDENT_BOTH,      // Head of line (spaces and tabs) MUST DIE
    TOKEN_INDENT_SPACE,     // Head of line (spaces)
    TOKEN_INDENT_TAB,       // Head of line (tabs)
    TOKEN_INLINE_COMMENT,
    TOKEN_ATOM,             // variable, function
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_SIGN,
    TOKEN_INFIX,
    TOKEN_CONSTANT,
    TOKEN_CONSTANT_INTEGER,
    TOKEN_CONSTANT_FLOAT,
    TOKEN_CONSTANT_EXPONENTIAL,
    TOKEN_CONSTANT_IMAGINARY,
    TOKEN_CONSTANT_WITH_LITERAL,
    TOKEN_CONSTANT_BIN,
    TOKEN_CONSTANT_OCT,
    TOKEN_CONSTANT_HEX,
    TOKEN_CONSTANT_64,
    TOKEN_AFTER_FLOATING_POINT,
    TOKEN_AFTER_IMAGINARY_UNIT,

    TOKEN_BLOCK_BEGIN_LBRACE,
    TOKEN_BLOCK_BEGIN_RBLACE,
    TOKEN_BLOCK_END_LBRACE,
    TOKEN_BLOCK_END_RBRACE,
    TOKEN_BLOCK_END_SLASH,
    TOKEN_BLOCK_MODIFIER,
    TOKEN_BLOCK_END_MODIFIER,
    TOKEN_BLOCK_STATE,
    TOKEN_BLOCK_INDENT,
    TOKEN_BLOCK_CONTENT,
};

Token::Token() :
    type(TOKEN_NOTYPE), line(0), column(0) {}
Token::Token(type_t t) :
    type(t), line(0), column(0) {}
Token::~Token() {}
void Token::setType(uint64_t t) { type = t; }
uint64_t Token::getType() { return type; }
void Token::setFlag(uint64_t flag) { status_flag |= flag; }
void Token::setPosition(size_t l, size_t c) { line = l; column = c; }
void Token::push(char c) { token.push_back(c); }
void Token::show() {
    printf("Type: ");
    switch (type) {
        case TOKEN_EOF:
            printf("EOF");
            break;
        case TOKEN_NOTYPE:
            printf("NOTYPE");
            break;
        case TOKEN_LINEFEED:
            printf("LINEFEED");
            break;
        case TOKEN_ATOM:
            printf("ATOM");
            break;
        case TOKEN_CONSTANT:
            printf("CONSTANT");
            break;
        case TOKEN_CONSTANT_INTEGER:
            printf("CONSTANT(INTEGER)");
            break;
        case TOKEN_CONSTANT_FLOAT:
            printf("CONSTANT(FLOAT)");
            break;
        case TOKEN_CONSTANT_EXPONENTIAL:
            printf("CONSTANT(EXPONENTIAL)");
            break;
        case TOKEN_CONSTANT_IMAGINARY:
            printf("CONSTANT(IMAGINARY)");
            break;
        case TOKEN_CONSTANT_BIN:
            printf("CONSTANT(BINARY)");
            break;
        case TOKEN_CONSTANT_OCT:
            printf("CONSTANT(OCTAL)");
            break;
        case TOKEN_CONSTANT_HEX:
            printf("CONSTANT(HEXDECIMAL)");
            break;
        case TOKEN_CONSTANT_64:
            printf("CONSTANT(BASE64)");
            break;
    }
    printf(", Token: \"");
    std::cout << token;
    printf("\"\n");
}

Lexer::Lexer() :
    lexing_block(false), line(1), column(0), cur(nullptr), eof(false), source(nullptr) {}
Lexer::~Lexer() {}

inline bool is_member_of(char c, char *array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (c == array[i]) {
            return true;
        }
    }
    return false;
}

inline bool isLowerCase(char c) {
    return ('a' <= c && c <= 'z');
}

inline bool isUpperCase(char c) {
    return ('A' <= c && c <= 'Z');
}

inline bool isMixedCase(char c) {
    return (isLowerCase(c) || isUpperCase(c));
}

inline bool isDigit(char c) {
    return ('0' <= c && c <= '9');
}

inline bool isChar(char c) {
    return (isMixedCase(c) || isDigit(c));
}

inline bool isZero(char c) {
    return c == '0';
}

inline bool isDigitNonZero(char c) {
    return ('1' <= c && c <= '9');
}

inline bool isBinLiteral(char c) {
    return c == 'b';
}

inline bool isBinDigit(char c) {
    return (c == '0' || c == '1');
}

inline bool isOctLiteral(char c) {
    return c == 'o';
}

inline bool isOctDigit(char c) {
    return ('0' <= c && c <= '7');
}

inline bool isHexLiteral(char c) {
    return c == 'x';
}

inline bool isHexDigit(char c) {
    return isDigit(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
}

inline bool is64Literal(char c) {
    return c == 'z';
}

// MIMEだと'='も含むが、二項演算子と見分けがつかないため
// Hydraの0zリテラルの定数のときは'='を使うことを禁止（文字列とコンバートするときに追加される）
inline bool is64Digit(char c) {
    return isChar(c) || c == '+' || c == '/';
}

inline bool isUnderScore(char c) {
    return c == '_';
}

inline bool isAtomChar(char c) {
    return (isChar(c) || isUnderScore(c));
}

inline bool isDot(char c) {
    return c == '.';
}

inline bool isSign(char c) {
    return (c == '+' || c == '-');
}

inline bool isImaginaryUnit(char c) {
    return c == 'j';
}

inline bool isExponential(char c) {
    return (c == 'e' || c == 'E');
}

inline bool isSymbol(char c) {
    char symbols[] = {'+', '-', '*', '/', '='};
    return is_member_of(c, symbols, sizeof(symbols));
}

inline bool isSpace(char c) {
    return c == ' ';
}

inline bool isTab(char c) {
    return c == '\t';
}

inline bool isSpaces(char c) {
    return isSpace(c) || isTab(c);
}

inline bool isAnySeparator(char c) {
    char seps[] = {' ', '\t', '\n', '\0', '(', ')', '{', '}', '[', ']'};
    return is_member_of(c, seps, sizeof(seps));
}

inline bool isSeparator(char c) {
    char seps[] = {' ', '\t', '\n', '\0'};
    return is_member_of(c, seps, sizeof(seps));
}

inline bool isLineFeed(char c) {
    return c == '\n';
}

inline bool isSharp(char c) {
    return c == '#';
}

inline bool isLParen(char c) {
    return c == '(';
}

inline bool isRParen(char c) {
    return c == ')';
}

inline bool isLBrace(char c) {
    return c == '{';
}

inline bool isRBrace(char c) {
    return c == '}';
}

inline bool isLBracket(char c) {
    return c == '[';
}

inline bool isRBracket(char c) {
    return c == ']';
}

inline bool isLParentheses(char c) {
    return isLParen(c) || isLBrace(c) || isLBracket(c);
}

inline bool isBackSlash(char c) {
    return c == '\\';
}

void Lexer::mod_BLOCK(std::vector<Token*> &tokens, char c, size_t column) {
    return;
}

// ブロック内の処理は別物
void Lexer::lexBlock(std::vector<Token*> &tokens, size_t line, size_t column) {
    // ブロックの終了はインデントで判断するため、mod_BLOCKに引数としてcolumnを受け渡す。
    while (source->get(c)) {
        switch (cur->getType()) {
            case: TOKEN_BLOCK_MODIFIER:
                break;
            case: TOKEN_BLOCK_STATE:
                break;
            case: TOKEN_BLOCK_INDENT:
                break;
            case: TOKEN_BLOCK_CONTENT:
                break;
            case: TOKEN_BLOCK_END_MODIFIER:
                break;
            case: TOKEN_BLOCK_END_SLASH:
                break;
            case: TOKEN_BLOCK_END_LBRACE:
                break;
            default:
                break;
        }
    }
}

void Lexer::separateWith(std::vector<Token*> &tokens, char c) {
    if (isLParen(c)) {
        cur->setType(TOKEN_LPAREN);
    } else if (isLBrace(c)) {
        cur->setType(TOKEN_LBRACE);
    } else if (isLBracket(c)) {
        cur->setType(TOKEN_LBRACKET);
    }

    tokens.push_back(cur);
    cur = nullptr;

    if (isLineFeed(c)) {
        // LINEFEEDを追加して行を加算し列をリセット
        tokens.push_back(new Token(TOKEN_LINEFEED));
        line++;
        column = 0;
    } else if (isRParen(c)) {
        tokens.push_back(new Token(TOKEN_RPAREN));
    } else if (isRBrace(c)) {
        tokens.push_back(new Token(TOKEN_RBRACE));
    } else if (isRBracket(c)) {
        tokens.push_back(new Token(TOKEN_LBRACKET));
    }
}


void Lexer::mod_NOTYPE(std::vector<Token*> &tokens, char c) {
    // cur にインスタンスを割り当てた直後のケース
    if (isSpaces(c)) {
        // 行頭ならばインデント
        // それ以外ならば無視すべき空白
        if (column == 1) {
            if isSpace(c) {
                cur->setType(TOKEN_INDENT_SPACE);
            } else if isTab(c) {
                cur->setType(TOKEN_INDENT_TAB)
            }
            cur->setPosition(line, column);
            cur->push(c);
        }
        return;
    } else if (isLineFeed(c)) {
        // 改行ならば改行トークンを追加
        cur->setType(TOKEN_LINEFEED);
        cur->setPosition(line, column);
        tokens.push_back(cur);
        cur = nullptr;
        line++;
        column = 0;
        return;
    } else if (isMixedCase(c)) {
        cur->setType(TOKEN_ATOM);
        cur->setPosition(line, column);
        cur->push(c);
        return;
    } else if (isZero(c)) {
        cur->setType(TOKEN_CONSTANT_WITH_LITERAL);
        cur->setPosition(line,column);
        cur->push(c);
        return;
    } else if (isDigitNonZero(c)) {
        cur->setType(TOKEN_CONSTANT);
        cur->setPosition(line, column);
        cur->push(c);
        return;
    } else if (isSharp(c)) {
        cur->setType(TOKEN_INLINE_COMMENT);
        cur->setPosition(line, column);
        cur->push(c);
        return;
    } else if (isSign(c)) {
        cur->setType(TOKEN_SIGN);
        cur->setPosition(line, column);
        cur->push(c);
        return;
    } else if (isSymbol(c)) {
        // +, - の場合は例外的に上のisSignで処理される
        cur->setType(TOKEN_INFIX);
        cur->push(c);
        return;
    } else if (isLBrace(c)) {
        cur->push(c);
        cur->setType(TOKEN_BLOCK_BEGIN_LBRACE);
        cur->setPosition(line, column);
        tokens.push_back(cur);
        cur = new Token(TOKEN_BLOCK_MODIFIER);
        lexBlock(tokens, line, column);
        return;
    } else {
        // エラー
        return;
    }
}

void Lexer::mod_CONSTANT_WITH_LITERAL(std::vector<Token*> &tokens, char c) {
    if (isBinLiteral(c)) {
        cur->setType(TOKEN_CONSTANT_BIN);
    } else if (isOctLiteral(c)) {
        cur->setType(TOKEN_CONSTANT_OCT);
    } else if (isHexLiteral(c)) {
        cur->setType(TOKEN_CONSTANT_HEX);
    } else if (is64Literal(c)) {
        cur->setType(TOKEN_CONSTANT_64);
    } else {
        // エラー 定義されていないリテラル
        printf("\n\nERROR: undefined literal\n\n");
    }
    cur->push(c);
    return;
}


void Lexer::mod_CONSTANT_BIN(std::vector<Token*> &tokens, char c) {
    if (isBinDigit(c)) {
        cur->push(c);
    } else if (isAnySeparator(c)) {
        if (isLParentheses(c)) {
            // エラー
            printf("\n\nERROR: binary digits\n\n");
        } else {
            separateWith(tokens, c);
        }
    } else {
        // エラー
        printf("\n\nERROR: parsing binary digits\n\n");
    }
    return;
}

void Lexer::mod_CONSTANT_OCT(std::vector<Token*> &tokens, char c) {
    if (isOctDigit(c)) {
        cur->push(c);
    } else if (isAnySeparator(c)) {
        if (isLParentheses(c)) {
            // エラー
            printf("\n\nERROR: parsing oct digits\n\n");
        } else {
            separateWith(tokens, c);
        }
    } else {
        // エラー
        printf("\n\nERROR: parsing oct digits\n\n");
    }
    return;
}

void Lexer::mod_CONSTANT_HEX(std::vector<Token*> &tokens, char c) {
    if (isHexDigit(c)) {
        cur->push(c);
    } else if (isAnySeparator(c)) {
        if (isLParentheses(c)) {
            // エラー
            printf("\n\nERROR: parsing hex decinal digits\n\n");
        } else {
            separateWith(tokens, c);
        }
    } else {
        // エラー
        printf("\n\nERROR: parsing hex decimal digits\n\n");
    }
    return;
}

void Lexer::mod_CONSTANT_64(std::vector<Token*> &tokens, char c) {
    if (is64Digit(c)) {
        cur->push(c);
    } else if (isAnySeparator(c)) {
        if (isLParentheses(c)) {
            // エラー
            printf("\n\nERROR: parsing base64 digits\n\n");
        } else {
            separateWith(tokens, c);
        }
    } else {
        // エラー
        printf("\n\nERROR: parsing base64 digits\n\n");
    }
    return;
}

void Lexer::mod_INLINE_COMMENT(std::vector<Token*> &tokens, char c) {
    if (isLineFeed(c)) {
        tokens.push_back(cur);
        cur = nullptr;
        tokens.push_back(new Token(TOKEN_LINEFEED));
        line++;
        column = 0;
        return;
    } else {
        cur->push(c);
    }
    return;
}

void Lexer::mod_CONSTANT(std::vector<Token*> &tokens, char c) {
    // 定数1個分を取り出したい
    if (isDigit(c)) {
        // 継続
        cur->push(c);
        return;
    } else if (isDot(c)) {
        // 小数点を検知
        cur->setType(TOKEN_AFTER_FLOATING_POINT);
        cur->push(c);
        return;
    } else if (isAnySeparator(c)) {
        // ずっとTOKEN_CONSTANTで処理してきたので、整数
        if (isLParentheses(c)) {
            // エラー
            printf("\n\nERROR: parsing integer\n\n");
        } else {
            cur->setType(TOKEN_CONSTANT_INTEGER);
            separateWith(tokens, c);
        }
        return;
    } else if (isImaginaryUnit(c)) {
        // 虚数。ここで終了
        cur->setType(TOKEN_CONSTANT_IMAGINARY);
        tokens.push_back(cur);
        cur = new Token(TOKEN_AFTER_IMAGINARY_UNIY);
        return;
    } else if (isExponential(c)) {
        // ここ実装してからコンパイル
        return;
    }
    /*else if (isSymbol(c)) {
        // 中置演算子と切り替え
    }*/ else {
        // エラー
        printf("\n\nERROR: parsing constant.\n\n");
        return;
    }
    return;
}

void Lexer::mod_CONSTANT_FLOAT(std::vector<Token*> &tokens, char c) {
    // 小数点を発見し、しかもその後が数字だった場合
    if (isDigit(c)) {
        cur->push(c);
    } else if (isImaginary(c)) {
        // ここ実装してからコンパイル

        return;
    } else if (isExponential(c)) {
        // ここ実装してからコンパイル
        return;
    } else if (isAnySeparator(c)) {
        if (isLParentheses(c)) {
            // エラー
            printf("\n\nERROR: parsing floating points\n\n");
        } else {
            separateWith(tokens, c);
        }
    } else {
        printf("\n\nERROR: paring float\n\n");
    }
    return;
}

void Lexer::mod_CONSTANT_EXPONENTIAL(std::vector<Token*> &tokens, char c) {
    return;
}

void Lexer::mod_SIGN(std::vector<Token*> &tokens, char c) {
    // 定数か中置演算子か判断したい
    if (isSymbol(c)) {
        // 中置演算子
        cur->setType(TOKEN_INFIX);
        cur->push(c);
    } else if (isDigit(c)) {
        // 定数
        tokens.push_back(cur);
        cur = new Token(TOKEN_CONSTANT);
        cur->push(c);
    } else if (isChar(c)) {
        // atom
        tokens.push_back(cur);
        cur = new Token(TOKEN_ATOM);
        cur->push(c);
    } else if (isLParentheses(c)) {
        tokens.push_back(cur);
        cur = new Token(TOKEN_LPAREN);
        cur->push(c);
        tokens.push_back(cur);
        cur = nullptr;
    } else {
        // エラーかと思われる
    }
    return;
}

void Lexer::mod_AFTER_FLOATING_POINT(std::vector<Token*> &tokens, char c) {
    if (isDigit(c)) {
        cur->setType(TOKEN_CONSTANT_FLOAT);
        cur->push(c);
    } else {
        // エラー
        printf("\n\nERROR: parsing floating point\n\n");
    }
}

void Lexer::mod_AFTER_IMAGINARY_UNIT(std::vector<Token*> &tokens, char c) {
    return;
}

void Lexer::mod_ATOM(std::vector<Token*> &tokens, char c) {
    // アトム1個分を取り出したい
    if (isAtomChar(c)) {
        cur->push(c);
        break;
    } else if (isAnySeparator(c)) {
        separateWith(tokens, c);
        break;
    } /*
    } else if (isSymbol(c)) {
        // 中置演算子と切り替え
        cur->setType(TOKEN_INFIX);
    } */
}

void Lexer::mod_INDENT_SPACE(std::vector<Token*> &tokens, char c) {
    // ここを実装してからコンパイル
    if (isSpace(c)) {
        cur->push(c);
    } else if (isTab(c)) {
        cur->setType(TOKEN_INDENT_BOTH);
        cur->push(c);
    } else {
        tokens.push_back(cur);

    }
}

void Lexer::mod_INDENT_TAB(std::vector<Token*> &tokens, char c) {
    if (isSpace(c)) {
        cur->setType(TOKEN_INDENT_BOTH);
        cur->push(c);
    } else if (isTab(c)) {
        cur->push(c);
    }
}

void Lexer::mod_INDENT_BOTH(std::vector<Token*> &tokens, char c) {
    if (isSpaces(c)) {
        cur->push(c);
    }
}

void Lexer::mod_LINEFEED(std::vector<Token*> &tokens, char c) {
    // lang1の構造上、ここには到達しないので到達したらエラー
}


void Lexer::mod_INFIX(std::vector<Token*> &tokens, char c) {
    if (isSymbol(c)) {
        // 継続
    } else if (isDigit) {
        // 定数に切り替え
    } else if (isMixedCase) {
        // atom に切り替え
    }
}

void Lexer::lexChar(std::vector<Token*> &tokens, char c) {
    if (eof) {
        if (cur != nullptr) {
            // ファイル終端で、かつトークンを処理し終わっていないとき
            // finalize したい
            tokens.push_back(cur);
            cur = nullptr;
        }
        tokens.push_back(new Token(TOKEN_EOF));
        return;
    }

    // cur にインスタンスが代入されていなければ初期化
    if (cur == nullptr) {
        cur = new Token();
    }

    // おそらくテキストファイルの仕様でファイル終端にはLFが追加されているようだ
    // if (isLineFeed(c)) printf("\\n");
    putchar(c);
    column++;

    switch (cur->getType()) {
        case TOKEN_NOTYPE:
            mod_NOTYPE(tokens, c);
            break;
        case TOKEN_INDENT_SPACE:
            mod_INDENT_SPACE(tokens, c);
            break;
        case TOKEN_INDENT_TAB:
            mod_INDENT_TAB(tokens, c);
            break;
        case TOKEN_INDENT_BOTH:
            mod_INDENT_BOTH(tokens, c);
            break;
        case TOKEN_LINEFEED:
            mod_LINEFEED(tokens, c);
            break;
        case TOKEN_INLINE_COMMENT:
            mod_INLINE_COMMENT(tokens, c);
            break;
        case TOKEN_ATOM:
            mod_ATOM(tokens, c);
            break;
        case TOKEN_CONSTANT_WITH_LITERAL:
            mod_CONSTANT_WITH_LITERAL(tokens, c);
            break;
        case TOKEN_CONSTANT_BIN:
            mod_CONSTANT_BIN(tokens, c);
            break;
        case TOKEN_CONSTANT_OCT:
            mod_CONSTANT_OCT(tokens, c);
            break;
        case TOKEN_CONSTANT_HEX:
            mod_CONSTANT_HEX(tokens, c);
            break;
        case TOKEN_CONSTANT_64:
            mod_CONSTANT_64(tokens, c);
            break;
        case TOKEN_AFTER_FLOATING_POINT:
            mod_AFTER_FLOATING_POINT(tokens, c);
            break;
        case TOKEN_CONSTANT_FLOAT:
            mod_CONSTANT_FLOAT(tokens, c);
            break;
        case TOKEN_CONSTANT:
            mod_CONSTANT(tokens, c);
            break;
        case TOKEN_CONSTANT_EXPONENTIAL:
            mod_CONSTANT_EXPONENTIAL(tokens, c);
            break;
        case TOKEN_AFTER_IMAGINARY_UNIT:
            mod_AFTER_INAGINARY_UNIT(tokens, c);
            break;
        case TOKEN_SIGN:
            mod_TOKEN_SIGN(tokens, c);
            break;
        case TOKEN_INFIX:
            mod_INFIX(tokens, c);
            break;
        default:
            printf("ERROR: unrecognized character.\n");
            break;
    }

}

void Lexer::lex(std::ifstream *src, std::vector<Token*> &tokens) {
    if (src == nullptr) {
        return;
    }

    if (src->eof()) {
        return;
    }

    // lex開始時にソースファイルをLexerにセット
    source = src;

    char c;
    while (source->get(c)) {
        lexChar(tokens, c);
    }

    eof = true;
    lexChar(tokens, '\0');

    // lex終了時にソースファイルをNULLにリセット（次のファイルに移れるようにするため）
    source = nullptr;
}

Parser::Parser() {}
Parser::~Parser() {}
void Parser::parseNextExpression(std::ifstream &source, AST &ast) {
    printf("parseNextExpression!!\n");
    std::string reading_buffer;
    reading_buffer.reserve(1024);
    std::getline(source, reading_buffer);
    std::cout << reading_buffer << std::endl;
}
