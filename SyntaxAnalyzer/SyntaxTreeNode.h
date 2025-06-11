#ifndef SYNTAXTREESTRUCTURE_H
#define SYNTAXTREESTRUCTURE_H

#include <vector>
#include "../Token.h"

class SyntaxTreeNode {
public:
    Token token;
    std::vector<SyntaxTreeNode*> children;

    SyntaxTreeNode(const Token& t) : token(t) {}

    void addChild(SyntaxTreeNode* child) {
        children.push_back(child);
    }

    ~SyntaxTreeNode() {
        for (auto child : children) {
            delete child;
        }
    }
};

#endif