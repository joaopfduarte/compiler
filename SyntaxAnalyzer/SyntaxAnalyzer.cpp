//
// Created by joaop on 6/8/2025.
//

#include "SyntaxAnalyzer.h"
#include<iostream>
#include<algorithm>

bool SyntaxAnalyzer::isValidFormat(const std::string& format) {
    return std::find(validFormats.begin(), validFormats.end(), format) != validFormats.end();
}

bool SyntaxAnalyzer::isValidDate(const std::string& date) {
    return date.find("/") != std::string::npos;
}

QueryType SyntaxAnalyzer::identifyQueryType(const std::queue<Token>& tokens) {
    auto tokensCopy = tokens;
    if (tokensCopy.empty()) return QueryType::UNKNOWN;

    std::string firstWord = tokensCopy.front().lexeme;
    std::transform(firstWord.begin(), firstWord.end(), firstWord.begin(), ::tolower);

    if (firstWord == "qual") {
        tokensCopy.pop();
        if (tokensCopy.empty()) return QueryType::UNKNOWN;

        std::string secondWord = tokensCopy.front().lexeme;
        std::transform(secondWord.begin(), secondWord.end(), secondWord.begin(), ::tolower);
        if (secondWord == "documento") {
            tokensCopy.pop();
            if (tokensCopy.empty()) return QueryType::UNKNOWN;

            std::string thirdWord = tokensCopy.front().lexeme;
            std::transform(thirdWord.begin(), thirdWord.end(), thirdWord.begin(), ::tolower);

            if (thirdWord == "está") return QueryType::FORMAT_QUERY;
            if (thirdWord == "tem") return QueryType::TITLE_QUERY;
        } else if (secondWord == "tamanho") {
            return QueryType::SIZE_QUERY;
        }
    } else if (firstWord == "quais" && !tokensCopy.empty()) {
        return QueryType::DATE_QUERY;
    } else if (firstWord == "mostre") {
        return QueryType::KEYWORD_QUERY;
    } else if (firstWord == "o" || firstWord == "quero" || firstWord == "só" || firstWord == "eu") {
        return QueryType::RESPONSE;
    }

    return QueryType::UNKNOWN;
}

ParsedQuery SyntaxAnalyzer::analyze(std::queue<Token> tokens) {
    QueryType type = identifyQueryType(tokens);
    currentQuery = ParsedQuery{type, {}, false, ""};

    switch (type) {
        case QueryType::FORMAT_QUERY:
            analyzeFormatQuery(tokens);
            break;
        case QueryType::SIZE_QUERY:
            analyzeSizeQuery(tokens);
            break;
        case QueryType::TITLE_QUERY:
            analyzeTitleQuery(tokens);
            break;
        case QueryType::DATE_QUERY:
            analyzeDateQuery(tokens);
            break;
        case QueryType::KEYWORD_QUERY:
            analyzeKeywordQuery(tokens);
            break;
        case QueryType::RESPONSE:
            analyzeResponse(tokens);
            break;
        default:
            std::cout << "Não entendi. \n";
            currentQuery.isComplete = false;
            break;
    }

    if (!currentQuery.isComplete) {
        queryHistory.push_back(currentQuery);
    }

    return currentQuery;
}

void SyntaxAnalyzer::analyzeFormatQuery(std::queue<Token> tokens) {
    for (int i = 0;i < 4; i++) {
        if (tokens.empty()) {
            currentQuery.isComplete = false;
            currentQuery.missingElement = "formato";
            return;
        }
        tokens.pop();
    }

    if (tokens.empty()) {
        currentQuery.isComplete = false;
        currentQuery.missingElement = "formato";
        return;
    }

    std::string format = tokens.front().lexeme;
    if (isValidFormat(format)) {
        currentQuery.parameters["formato"] = format;
        currentQuery.isComplete = true;
    } else {
        std::cout << "Formato inválido: " << format << std::endl;
        currentQuery.isComplete = false;
        currentQuery.missingElement = "formato";
    }
}

std::string SyntaxAnalyzer::getMissingElement() const {
    return currentQuery.missingElement;
}

bool SyntaxAnalyzer::handleResponse(std::queue<Token> tokens) {
    if (queryHistory.empty())
        std::cout << "Não há nenhuma consulta anterior.\n";
        return false;

    ParsedQuery lastQuery = queryHistory.back();

    return true;
}
