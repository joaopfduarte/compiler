//
// Created by joaop on 7/17/2025.
//

#include "Translator.h"

#include "../SyntaxAnalyzer/SyntaxAnalyzer.h"

std::string Translator::translateToIEEEQuery(SyntaxTreeNode *root) {
    if (!root) return "";

    std::string baseUrl = "https://ieeexplore.ieee.org/search/searchresult.jsp";
    std::string queryParams = "?action=search&newsearch=true&matchBoolean=true";

    std::string queryText = "&queryText=(" + translateNode(root) + ")";

    std::string filters;
    for (auto &child: root->children) {
        if (child->token.lexeme == "ranges") {
            filters = formatFilters(child->token.lexeme);
        }
    }

    return baseUrl + queryParams + queryText + filters;
}

std::string Translator::translateNode(SyntaxTreeNode *node) {
    if (!node) return "";

    std::string result;

    for (size_t i = 0; i < node->children.size(); i++) {
        SyntaxTreeNode *child = node->children[i];

        if (result.size() > 0) {
            result += (node->token.lexeme == "AND") ? " AND " : " OR ";
        }

        if (child->token.type == "FIELD") {
            if (child->children.size() > 0) {
                result += "\"" + child->token.lexeme + "\":\"" + child->children[0]->token.lexeme + "\"";
            }
        } else {
            result += translateNode(child);
        }
    }
    return result;
}

    std::string Translator::formatFilters(const std::string &range) {
        return "&ranges=" + range + "_Year";
    }
