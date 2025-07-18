#include "Translator.h"
#include "../SyntaxAnalyzer/SyntaxAnalyzer.h"

std::string Translator::translateToIEEEQuery(SyntaxTreeNode *root, QueryType queryType) {
    if (!root) return "";

    std::string baseUrl = "https://ieeexplore.ieee.org/search/searchresult.jsp";
    std::string queryParams = "?action=search&newsearch=true&matchBoolean=true";

    std::string queryText;

    SyntaxTreeNode *lastRelevantNode = nullptr;
    SyntaxTreeNode *currentNode = root;

    while (!currentNode->children.empty()) {
        currentNode = currentNode->children.back();
        if (!currentNode->token.lexeme.empty()) {
            lastRelevantNode = currentNode;
        }
    }

    if (lastRelevantNode) {
        if (queryType == QueryType::KEYWORD_QUERY) {
            queryText = "&queryText=" + lastRelevantNode->token.lexeme;
        } else if (queryType == QueryType::TITLE_QUERY) {
            queryText = "&queryText=Título:%20" + lastRelevantNode->token.lexeme;
        } else {
            queryText = "&queryText=Consulta não suportada";
        }
    } else {
        queryText = "undefined";
    }

    return baseUrl + queryParams + queryText;

}

std::string Translator::processKeywordQuery(SyntaxTreeNode *root) {
    if (!root) return "";

    std::string queryText = "&queryText=";

    SyntaxTreeNode *lastRelevantNode = nullptr;

    for (auto child: root->children) {
        if (!child->token.lexeme.empty()) {
            lastRelevantNode = child;
        }
    }

    if (lastRelevantNode) {
        queryText += lastRelevantNode->token.lexeme;
    } else {
        queryText += "undefined";
    }

    return queryText;
}

std::string Translator::processTitleQuery(SyntaxTreeNode *root) {
    if (!root) return "";

    std::string queryText = "&queryText=Título:%20";

    SyntaxTreeNode *lastRelevantNode = nullptr;

    for (auto child: root->children) {
        if (!child->token.lexeme.empty()) {
            lastRelevantNode = child;
        }
    }

    if (lastRelevantNode) {
        queryText += lastRelevantNode->token.lexeme;
    } else {
        queryText += "undefined";
    }

    return queryText;
}

