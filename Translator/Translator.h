//
// Created by joaop on 7/17/2025.
//

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <string>
#include <map>
#include "../Token.h"
#include "../SyntaxAnalyzer/SyntaxAnalyzer.h"

struct ParsedQuery;

class Translator {
public:
    Translator() = default;
    std::string translateToIEEEQuery(SyntaxTreeNode* node);
private:
    std::string translateNode(SyntaxTreeNode* node);
    std::string formatFilters(const std::string& range);
};
#endif //TRANSLATOR_H
