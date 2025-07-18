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
    std::string translateToIEEEQuery(SyntaxTreeNode* node, QueryType type);
private:
    std::string processKeywordQuery(SyntaxTreeNode *root);
    std::string processTitleQuery(SyntaxTreeNode *root);
};
#endif //TRANSLATOR_H
