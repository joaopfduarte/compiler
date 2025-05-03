#ifndef TOKEN_H
#define TOKEN_H

#include <string>

struct Token {
    std::string lexeme;
    std::string type;
    int line;
    int column;
    
    Token(std::string l, std::string t, int lin, int col) 
        : lexeme(std::move(l)), type(std::move(t)), line(lin), column(col) {}
};

#endif