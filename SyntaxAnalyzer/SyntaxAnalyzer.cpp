#include "SyntaxAnalyzer.h"
#include<iostream>
#include<algorithm>
#include<fstream>
#include<filesystem>

/*
 *Método para validação de logs
 */

std::string QueryTypeToString(QueryType type) {
    switch (type) {
        case QueryType::FORMAT_QUERY:
            return "FORMAT_QUERY";
        case QueryType::SIZE_QUERY:
            return "SIZE_QUERY";
        case QueryType::TITLE_QUERY:
            return "TITLE_QUERY";
        case QueryType::DATE_QUERY:
            return "DATE_QUERY";
        case QueryType::KEYWORD_QUERY:
            return "KEYWORD_QUERY";
        case QueryType::RESPONSE:
            return "RESPONSE";
        case QueryType::UNKNOWN:
            return "UNKNOWN";
        default:
            return "INVALID_TYPE";
    }
}

bool SyntaxAnalyzer::isValidFormat(const std::string &format) {
    /*
        Verifica se o formato começa com um ponto (.)
        */
    std::string validatedFormat = format;

    /*
    Valida se o formato ajustado existe na lista validFormats
    */
    return std::find(validFormats.begin(), validFormats.end(), validatedFormat) != validFormats.end();
}


bool SyntaxAnalyzer::isValidDate(const std::string &date) {
    return date.find("/") != std::string::npos;
}

QueryType SyntaxAnalyzer::identifyQueryType(const std::queue<Token> &tokens) {
    auto tokensCopy = tokens;
    if (tokensCopy.empty()) {
        std::cout << "[DEBUG] Fila de tokens vazia em identifyQueryType." << std::endl;
        return QueryType::UNKNOWN;
    }

    /*
    Primeiro token (primeira palavra)
    */
    std::string firstWord = tokensCopy.front().lexeme;
    std::transform(firstWord.begin(), firstWord.end(), firstWord.begin(), ::tolower);
    std::cout << "[DEBUG] Primeira palavra após transformação: '" << firstWord << "'" << std::endl;

    /*
    Método para identificar o tipo de consulta no caso de tokens iniciados com "Qual".
    */
    if (firstWord == "qual") {
        tokensCopy.pop();

        if (tokensCopy.empty()) {
            std::cout << "[DEBUG] Fila de tokens ficou vazia após 'Qual'." << std::endl;
            return QueryType::UNKNOWN;
        }

        /*
        Segundo token (Segunda palavra)
        */
        std::string secondWord = tokensCopy.front().lexeme;
        std::transform(secondWord.begin(), secondWord.end(), secondWord.begin(), ::tolower);
        std::cout << "[DEBUG] Segunda palavra após transformação: '" << secondWord << "'" << std::endl;

        if (secondWord == "documento") {
            /*
            Caso: "Qual documento ..."
            */
            tokensCopy.pop();

            if (tokensCopy.empty()) {
                std::cout << "[DEBUG] Fila de tokens esvaziada ao analisar 'documento'." << std::endl;
                return QueryType::UNKNOWN;
            }

            /*
            Terceiro token (Terceira palavra)
            */
            std::string thirdWord = tokensCopy.front().lexeme;
            std::transform(thirdWord.begin(), thirdWord.end(), thirdWord.begin(), ::tolower);
            std::cout << "[DEBUG] Terceira palavra após transformação: '" << thirdWord << "'" << std::endl;

            if (thirdWord == "está" || thirdWord == "esta") {
                /*
                Se for: "Qual documento está no <formato>?"
                */
                tokensCopy.pop();

                if (!tokensCopy.empty() && tokensCopy.front().lexeme == "no") {
                    tokensCopy.pop();

                    if (!tokensCopy.empty()) {
                        Token nextToken = tokensCopy.front();
                        std::cout << "[DEBUG] Próximo token: " << nextToken.lexeme << std::endl;


                        if (isValidFormat(nextToken.lexeme)) {
                            tokensCopy.pop();
                            std::cout << "[DEBUG] Query identificada como FORMAT_QUERY com formato: "
                                    << nextToken.lexeme << std::endl;
                            currentQuery.parameters["formato"] = nextToken.lexeme;
                            return QueryType::FORMAT_QUERY;
                        } else {
                            std::cout << "[DEBUG] Formato inválido: " << nextToken.lexeme << std::endl;
                            return QueryType::UNKNOWN;
                        }
                    } else {
                        std::cout << "[DEBUG] Fim da consulta encontrado sem especificar o formato.\n";
                        return QueryType::UNKNOWN;
                    }
                }
            } else if (thirdWord == "tem") {
                /*
                Se for: "Qual documento tem o título <titulo>?"
                */
                tokensCopy.pop();

                if (!tokensCopy.empty()) {
                    std::string nextWord = tokensCopy.front().lexeme;
                    std::transform(nextWord.begin(), nextWord.end(), nextWord.begin(), ::tolower);

                    if (nextWord == "o" || nextWord == "título" || nextWord == "titulo") {
                        std::cout << "[DEBUG] Query identificada como TITLE_QUERY." << std::endl;
                        return QueryType::TITLE_QUERY;
                    }
                }
            }
        } else if (secondWord == "tamanho") {
            /*
            Caso: "Qual tamanho tem o documento <titulo>?"
            */
            tokensCopy.pop();

            if (!tokensCopy.empty()) {
                std::string thirdWord = tokensCopy.front().lexeme;
                std::transform(thirdWord.begin(), thirdWord.end(), thirdWord.begin(), ::tolower);
                std::cout << "[DEBUG] Terceira palavra após transformação: '" << thirdWord << "'" << std::endl;

                if (thirdWord == "tem") {
                    tokensCopy.pop();

                    if (!tokensCopy.empty() && tokensCopy.front().lexeme == "o") {
                        tokensCopy.pop();
                        std::cout << "[DEBUG] Query identificada como SIZE_QUERY." << std::endl;
                        return QueryType::SIZE_QUERY;
                    }
                }
            }
        }
    } else if (firstWord == "mostre") {
        tokensCopy.pop();

        if (tokensCopy.empty()) {
            std::cout << "[DEBUG] Fila de tokens ficou vazia após 'mostre'." << std::endl;
            return QueryType::UNKNOWN;
        }

        /*
        Segundo token (Segunda palavra)
        */
        std::string secondWord = tokensCopy.front().lexeme;
        std::transform(secondWord.begin(), secondWord.end(), secondWord.begin(), ::tolower);
        std::cout << "[DEBUG] Segunda palavra após transformação: '" << secondWord << "'" << std::endl;

        if (secondWord == "documentos" || secondWord == "documento") {
            std::cout << "[DEBUG] Query identificada como KEYWORD_QUERY." << std::endl;
            return QueryType::KEYWORD_QUERY;
        }
    } else if (firstWord == "quais") {
        /*
        Consultas que buscam documentos por data
        */
        tokensCopy.pop();

        if (!tokensCopy.empty()) {
            std::string secondWord = tokensCopy.front().lexeme;
            std::transform(secondWord.begin(), secondWord.end(), secondWord.begin(), ::tolower);
            tokensCopy.pop();

            if (secondWord == "documentos") {
                if (!tokensCopy.empty()) {
                    std::string thirdWord = tokensCopy.front().lexeme;
                    std::transform(thirdWord.begin(), thirdWord.end(), thirdWord.begin(), ::tolower);
                    if (thirdWord == "foram") {
                        std::cout << "[DEBUG] Query identificada como DATE_QUERY." << std::endl;
                        return QueryType::DATE_QUERY;
                    }
                }
            }
        }
    }


    std::cout << "[DEBUG] Nenhum padrão reconhecido. Retornando UNKNOWN." << std::endl;
    return QueryType::UNKNOWN;
}

ParsedQuery SyntaxAnalyzer::analyze(std::queue<Token> tokens) {
    std::cout << "Inicializando categorização de token: " << std::endl;
    QueryType type = identifyQueryType(tokens);
    std::cout << "[DEBUG] Identified query type: " << QueryTypeToString(type) << std::endl;

    currentQuery = ParsedQuery{type, {}, false, ""};

    buildSyntaxTree(tokens);

    std::cout << "[DEBUG] Árvore de Sintaxe Construída:\n" << std::endl;
    printSyntaxTree(root);

    switch (type) {
        case QueryType::FORMAT_QUERY:
            std::cout << "[DEBUG] Analyzing format query..." << std::endl;
            analyzeFormatQuery(tokens);
            break;
        case QueryType::SIZE_QUERY:
            std::cout << "[DEBUG] Analyzing size query..." << std::endl;
            analyzeSizeQuery(tokens);
            break;
        case QueryType::TITLE_QUERY:
            std::cout << "[DEBUG] Analyzing title query..." << std::endl;
            analyzeTitleQuery(tokens);
            break;
        case QueryType::DATE_QUERY:
            std::cout << "[DEBUG] Analyzing date query..." << std::endl;
            analyzeDateQuery(tokens);
            break;
        case QueryType::KEYWORD_QUERY:
            std::cout << "[DEBUG] Analyzing keyword query..." << std::endl;
            analyzeKeywordQuery(tokens);
            break;
        case QueryType::RESPONSE:
            std::cout << "[DEBUG] Analyzing response..." << std::endl;
            analyzeResponse(tokens);
            break;
        default:
            std::cout << "[DEBUG] Nao entendi" << std::endl;
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
    /*
         Verifica se há tokens para processar
    */
    if (tokens.empty()) {
        std::cout << "Resposta vazia.\n";
        return false;
    }
    /*
         Obtém a primeira palavra da resposta
    */
    std::string firstWord = tokens.front().lexeme;
    std::transform(firstWord.begin(), firstWord.end(), firstWord.begin(), ::tolower);

    /*
     Processa a resposta com base no formato esperado
*/
    switch (lastQuery.type) {
        case QueryType::FORMAT_QUERY: {
            /*
                        Espera: "O formato é <formato>"
            */
            if (firstWord != "o") {
                std::cout << "Resposta deve começar com 'O formato é...'\n";
                return false;
            }
            /*
                         Remove "O formato é"
            */
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
            /*
                Espera uma resposta no formato: "Quero tamanho maior que <numero>"
            */

            if (tokens.empty()) {
                std::cout << "A resposta está vazia.\n";
                return false;
            }

            std::string currentWord = tokens.front().lexeme;
            std::transform(currentWord.begin(), currentWord.end(), currentWord.begin(), ::tolower);

            if (currentWord != "quero") {
                std::cout << "Resposta deve começar com 'Quero tamanho maior que...'\n";
                return false;
            }

            tokens.pop();

            if (tokens.empty() || tokens.front().lexeme != "tamanho") {
                std::cout << "Esperava 'tamanho' após 'Quero'.\n";
                return false;
            }

            tokens.pop();

            if (tokens.empty() || tokens.front().lexeme != "maior") {
                std::cout << "Esperava 'maior' após 'tamanho'.\n";
                return false;
            }

            tokens.pop();

            if (tokens.empty() || tokens.front().lexeme != "que") {
                std::cout << "Esperava 'que' após 'maior'.\n";
                return false;
            }

            tokens.pop();

            if (tokens.empty()) {
                std::cout << "Tamanho não especificado após 'que'.\n";
                return false;
            }

            currentWord = tokens.front().lexeme;
            tokens.pop();

            if (!std::all_of(currentWord.begin(), currentWord.end(), ::isdigit)) {
                std::cout << "Tamanho inválido: " << currentWord << "\n";
                return false;
            }

            lastQuery.parameters["tamanho"] = currentWord;
            lastQuery.isComplete = true;

            std::cout << "Consulta de tamanho processada com sucesso. Tamanho especificado: " << currentWord <<
                    " bytes.\n";

            return true;
        }

        case QueryType::DATE_QUERY: {
            /*
                        Espera: "Só documentos após <data>"
            */
            if (firstWord != "só") {
                std::cout << "Resposta deve começar com 'Só documentos após...'\n";
                return false;
            }

            /*

*/
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
            /*
                        Espera: "Eu quis dizer <palavra_chave>"
            */
            if (firstWord != "eu") {
                std::cout << "Resposta deve começar com 'Eu quis dizer...'\n";
                return false;
            }

            /*
            Remove "Eu quis dizer"
*/

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

            /*
                        Remove espaço extra no final
            */
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

size_t getFileSize(const std::string &filePath) {
    std::cout << "[DEBUG] Tentando abrir arquivo: " << filePath << "\n";
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!file) {
        std::cout << "[DEBUG] Arquivo não encontrado ou falha na abertura: " << filePath << "\n";
        return static_cast<size_t>(-1);
    }
    return static_cast<size_t>(file.tellg());
}

void SyntaxAnalyzer::analyzeSizeQuery(std::queue<Token> tokens) {
    /*
        Qual o tamanho do documento <titulo>?
    */

    for (int i = 0; i < 5; i++) {
        if (tokens.empty()) {
            currentQuery.isComplete = false;
            currentQuery.missingElement = "título";
            return;
        }
        tokens.pop();
    }

    std::string title;
    while (!tokens.empty() && tokens.front().lexeme != "?") {
        title += tokens.front().lexeme;
        tokens.pop();
    }


    if (tokens.empty()) {
        currentQuery.isComplete = false;
        currentQuery.missingElement = "título";
        return;
    }

    /*
    Captura o título do doc
*/

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

    /*
    Armazena o título na consulta
*/

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
    /*
        Remove "Quais documentos foram criados em"
    */

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
    /*
     Remove "Mostre documentos sobre"
*/

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
    /*
        chamado quando receber uma resposta
    */

    currentQuery.isComplete = false;
}

std::string SyntaxAnalyzer::generateResponse(const ParsedQuery &query) {
    switch (query.type) {
        case QueryType::FORMAT_QUERY: {
            auto it = query.parameters.find("formato");
            if (it != query.parameters.end()) {
                return "O formato é: " + it->second;
            }
            return "Não foi possível identificar o formato.";
        }
        case QueryType::SIZE_QUERY: {
            auto it = query.parameters.find("título");
            /* if (it != query.parameters.end()) {
                std::string relativePath = "SyntaxAnalyzer/" + it->second;

                std::filesystem::path projectRoot = std::filesystem::current_path().parent_path();
                std::filesystem::path filePath = projectRoot / relativePath;

                filePath = std::filesystem::canonical(filePath);

                std::cout << "[DEBUG] Caminho completo do arquivo: " << filePath.string() << "\n";

                size_t fileSize = getFileSize(filePath.string());
                if (fileSize == static_cast<size_t>(-1)) {
                    return "O arquivo \"" + filePath.string() + "\" não foi encontrado no sistema.";
                }

                return "O tamanho do arquivo \"" + filePath.string() + "\" é: " + std::to_string(fileSize) + " bytes.";
            } */
            return "Não foi possível identificar o caminho do arquivo.";
        }
        case QueryType::TITLE_QUERY: {
            auto it = query.parameters.find("título");
            if (it != query.parameters.end()) {
                return "O título correto é: \"" + it->second + "\".";
            }
            return "Não foi possível identificar o título.";
        }
        case QueryType::DATE_QUERY: {
            auto it = query.parameters.find("data");
            if (it != query.parameters.end()) {
                return "Só documentos após " + it->second + ".";
            }
            return "Não foi possível identificar a data.";
        }
        case QueryType::KEYWORD_QUERY: {
            auto it = query.parameters.find("palavra_chave");
            if (it != query.parameters.end()) {
                return "Os documentos relacionados à palavra-chave \"" + it->second + "\" serão listados.";
            }
            return "Não foi possível identificar a palavra-chave.";
        }
        case QueryType::RESPONSE: {
            return "Resposta processada com sucesso.";
        }
        default: {
            return "Consulta desconhecida.";
        }
    }
}

/*
 * Syntax Tree implementation and print
 */
void SyntaxAnalyzer::buildSyntaxTree(std::queue<Token> tokens) {
    if (root) {
        delete root;
        root = nullptr;
    }

    if (tokens.empty()) {
        std::cout << "[DEBUG] Nenhum token fornecido para construir a árvore de sintaxe.\n";
        return;
    }

    root = new SyntaxTreeNode(tokens.front());
    tokens.pop();

    SyntaxTreeNode *currentNode = root;
    while (!tokens.empty()) {
        SyntaxTreeNode *child = new SyntaxTreeNode(tokens.front());
        tokens.pop();

        currentNode->addChild(child);


        currentNode = child;
    }
}

void SyntaxAnalyzer::printSyntaxTree(SyntaxTreeNode *node, int depth) const {
    if (!node) {
        return;
    }

    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
    std::cout << node->token.lexeme << " (" << node->token.type << ")\n";

    for (auto child: node->children) {
        printSyntaxTree(child, depth + 1);
    }
}
