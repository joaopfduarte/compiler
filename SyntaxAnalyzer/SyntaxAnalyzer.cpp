//
// Created by joaop on 6/8/2025.
//

#include "SyntaxAnalyzer.h"
#include<iostream>
#include<algorithm>

bool SyntaxAnalyzer::isValidFormat(const std::string &format) {
    return std::find(validFormats.begin(), validFormats.end(), format) != validFormats.end();
}

bool SyntaxAnalyzer::isValidDate(const std::string &date) {
    return date.find("/") != std::string::npos;
}

QueryType SyntaxAnalyzer::identifyQueryType(const std::queue<Token> &tokens) {
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
            std::cout << "Nao entendi. \n";
            currentQuery.isComplete = false;
            break;
    }

    if (!currentQuery.isComplete) {
        queryHistory.push_back(currentQuery);
    }

    return currentQuery;
}

void SyntaxAnalyzer::analyzeFormatQuery(std::queue<Token> tokens) {
    for (int i = 0; i < 4; i++) {
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
    if (queryHistory.empty()) {
        std::cout << "Não há consulta pendente para responder.\n";
        return false;
    }

    ParsedQuery &lastQuery = queryHistory.back();

    // Verifica se há tokens para processar
    if (tokens.empty()) {
        std::cout << "Resposta vazia.\n";
        return false;
    }

    // Obtém a primeira palavra da resposta
    std::string firstWord = tokens.front().lexeme;
    std::transform(firstWord.begin(), firstWord.end(), firstWord.begin(), ::tolower);

    // Processa a resposta com base no formato esperado
    switch (lastQuery.type) {
        case QueryType::FORMAT_QUERY: {
            // Espera: "O formato é <formato>"
            if (firstWord != "o") {
                std::cout << "Resposta deve começar com 'O formato é...'\n";
                return false;
            }

            // Remove "O formato é"
            for (int i = 0; i < 3 && !tokens.empty(); i++) {
                tokens.pop();
            }

            if (tokens.empty()) {
                std::cout << "Formato não especificado.\n";
                return false;
            }

            std::string format = tokens.front().lexeme;
            if (isValidFormat(format)) {
                lastQuery.parameters["formato"] = format;
                lastQuery.isComplete = true;
                return true;
            } else {
                std::cout << "Formato inválido: " << format << std::endl;
                return false;
            }
        }

        case QueryType::SIZE_QUERY: {
            // Espera: "Quero tamanho maior que <numero>"
            if (firstWord != "quero") {
                std::cout << "Resposta deve começar com 'Quero tamanho maior que...'\n";
                return false;
            }

            // Remove "Quero tamanho maior que"
            for (int i = 0; i < 4 && !tokens.empty(); i++) {
                tokens.pop();
            }

            if (tokens.empty()) {
                std::cout << "Tamanho não especificado.\n";
                return false;
            }

            std::string size = tokens.front().lexeme;
            // Verifica se é um número
            if (std::all_of(size.begin(), size.end(), ::isdigit)) {
                lastQuery.parameters["tamanho"] = size;
                lastQuery.isComplete = true;
                return true;
            } else {
                std::cout << "Tamanho inválido: " << size << std::endl;
                return false;
            }
        }

        case QueryType::DATE_QUERY: {
            // Espera: "Só documentos após <data>"
            if (firstWord != "só") {
                std::cout << "Resposta deve começar com 'Só documentos após...'\n";
                return false;
            }

            // Remove "Só documentos após"
            for (int i = 0; i < 3 && !tokens.empty(); i++) {
                tokens.pop();
            }

            if (tokens.empty()) {
                std::cout << "Data não especificada.\n";
                return false;
            }

            std::string date;
            while (!tokens.empty()) {
                date += tokens.front().lexeme + " ";
                tokens.pop();
            }

            if (isValidDate(date)) {
                lastQuery.parameters["data"] = date;
                lastQuery.isComplete = true;
                return true;
            } else {
                std::cout << "Data inválida: " << date << std::endl;
                return false;
            }
        }

        case QueryType::KEYWORD_QUERY: {
            // Espera: "Eu quis dizer <palavra_chave>"
            if (firstWord != "eu") {
                std::cout << "Resposta deve começar com 'Eu quis dizer...'\n";
                return false;
            }

            // Remove "Eu quis dizer"
            for (int i = 0; i < 3 && !tokens.empty(); i++) {
                tokens.pop();
            }

            if (tokens.empty()) {
                std::cout << "Palavra-chave não especificada.\n";
                return false;
            }

            std::string keyword;
            while (!tokens.empty()) {
                keyword += tokens.front().lexeme + " ";
                tokens.pop();
            }

            // Remove espaço extra no final
            if (!keyword.empty() && keyword.back() == ' ') {
                keyword.pop_back();
            }

            lastQuery.parameters["palavra_chave"] = keyword;
            lastQuery.isComplete = true;
            return true;
        }

        default:
            std::cout << "Tipo de consulta não suporta resposta.\n";
            return false;
    }
}

void SyntaxAnalyzer::analyzeSizeQuery(std::queue<Token> tokens) {
    //Qual o tamanho do documento <titulo>?

    for (int i = 0; i < 5; i++) {
        if (tokens.empty()) {
            currentQuery.isComplete = false;
            currentQuery.missingElement = "título";
            return;
        }
        tokens.pop();
    }

    if (tokens.empty()) {
        currentQuery.isComplete = false;
        currentQuery.missingElement = "título";
        return;
    }

    //Captura o título do doc
    std::string title;
    while (!tokens.empty() && tokens.front().lexeme != "?") {
        title += tokens.front().lexeme + " ";
        tokens.pop();
    }

    if (!title.empty() && title[title.length() - 1] == ' ') {
        title = title.substr(0, title.length() - 1);
    }

    if (title.empty()) {
        currentQuery.isComplete = false;
        currentQuery.missingElement = "título";
        return;
    }

    //Armazena o título na consulta
    currentQuery.parameters["título"] = title;
    currentQuery.isComplete = true;
}

void SyntaxAnalyzer::analyzeTitleQuery(std::queue<Token> tokens) {
    for (int i = 0; i < 5; i++) {
        if (tokens.empty()) {
            currentQuery.isComplete = false;
            currentQuery.missingElement = "título";
            return;
        }
        tokens.pop();
    }

    if (tokens.empty()) {
        currentQuery.isComplete = false;
        currentQuery.missingElement = "título";
        return;
    }

    std::string title;
    while (!tokens.empty() && tokens.front().lexeme != "?") {
        title += tokens.front().lexeme + " ";
        tokens.pop();
    }

    if (!title.empty() && title[title.length() - 1] == ' ') {
        title = title.substr(0, title.length() - 1);
    }

    if (title.empty()) {
        currentQuery.isComplete = false;
        currentQuery.missingElement = "título";
        return;
    }

    if (title.front() == '"' && title.back() == '"') {
        title = title.substr(1, title.length() - 2);
    }

    currentQuery.parameters["título"] = title;
    currentQuery.isComplete = true;
}

void SyntaxAnalyzer::analyzeDateQuery(std::queue<Token> tokens) {
    // Remove "Quais documentos foram criados em"
    for (int i = 0; i < 5; i++) {
        if (tokens.empty()) {
            currentQuery.isComplete = false;
            currentQuery.missingElement = "data";
            return;
        }
        tokens.pop();
    }

    if (tokens.empty()) {
        currentQuery.isComplete = false;
        currentQuery.missingElement = "data";
        return;
    }

    std::string date;
    while (!tokens.empty() && tokens.front().lexeme != "?") {
        date += tokens.front().lexeme;
        tokens.pop();
    }

    if (isValidDate(date)) {
        currentQuery.parameters["data"] = date;
        currentQuery.isComplete = true;
    } else {
        currentQuery.isComplete = false;
        currentQuery.missingElement = "data";
    }
}

void SyntaxAnalyzer::analyzeKeywordQuery(std::queue<Token> tokens) {
    // Remove "Mostre documentos sobre"
    for (int i = 0; i < 3; i++) {
        if (tokens.empty()) {
            currentQuery.isComplete = false;
            currentQuery.missingElement = "palavra_chave";
            return;
        }
        tokens.pop();
    }

    if (tokens.empty()) {
        currentQuery.isComplete = false;
        currentQuery.missingElement = "palavra_chave";
        return;
    }

    std::string keyword;
    while (!tokens.empty() && tokens.front().lexeme != "?") {
        keyword += tokens.front().lexeme + " ";
        tokens.pop();
    }

    if (!keyword.empty() && keyword.back() == ' ') {
        keyword = keyword.substr(0, keyword.length() - 1);
    }

    if (!keyword.empty()) {
        currentQuery.parameters["palavra_chave"] = keyword;
        currentQuery.isComplete = true;
    } else {
        currentQuery.isComplete = false;
        currentQuery.missingElement = "palavra_chave";
    }
}

void SyntaxAnalyzer::analyzeResponse(std::queue<Token> tokens) {
    //chamado quando receber uma resposta
    currentQuery.isComplete = false;
}
