#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <algorithm>
#include "Token.h"

class LexicalAnalyzer {
private:
    std::set<std::string> stopwords;
    std::vector<std::string> symbolTable;
    std::queue<Token> tokenQueue;

    // Conjunto de caracteres válidos
    std::string validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
                            ".,;:!?()[]{}+-*/=><\"'@#$%&_\\| \t\n\r";

public:
    LexicalAnalyzer() {
        loadStopwords();
    }

    void loadStopwords();

    bool isValidChar(char c) const;

    int levenshteinDistance(const std::string& s1, const std::string& s2);

    std::string findSimilarWord(const std::string& word);

    void analyze(const std::string& text);

    const std::vector<std::string>& getSymbolTable() const { return symbolTable; }
    std::queue<Token> getTokenQueue() const { return tokenQueue; }
};

#endif
#include "LexicalAnalyzer.h"
#include <fstream>
#include <sstream>
#include <iostream>

void LexicalAnalyzer::loadStopwords() {
    // Fonte das stopwords: https://gist.github.com/alopes/5358189
    std::vector<std::string> defaultStopwords = {
        "a", "ao", "aos", "aquela", "aquelas", "aquele", "aqueles", "aquilo",
        "as", "até", "com", "como", "da", "das", "de", "dela", "delas", "dele",
        "deles", "depois", "do", "dos", "e", "ela", "elas", "ele", "eles", "em",
        "entre", "era", "eram", "essa", "essas", "esse", "esses", "esta", "estas",
        "este", "estes", "eu", "foi", "foram", "há", "isso", "isto", "já", "lhe",
        "lhes", "mais", "mas", "me", "mesmo", "meu", "meus", "minha", "minhas",
        "muito", "na", "nas", "nem", "no", "nos", "nossa", "nossas", "nosso",
        "nossos", "num", "numa", "o", "os", "ou", "para", "pela", "pelas", "pelo",
        "pelos", "por", "qual", "quando", "que", "quem", "são", "se", "seja",
        "sem", "seu", "seus", "só", "sua", "suas", "também", "te", "tem", "têm",
        "seu", "sua", "teu", "tua", "teus", "tuas", "um", "uma", "umas", "uns"
    };
    
    stopwords = std::set<std::string>(defaultStopwords.begin(), defaultStopwords.end());
}

bool LexicalAnalyzer::isValidChar(char c) const {
    return validChars.find(c) != std::string::npos;
}

int LexicalAnalyzer::levenshteinDistance(const std::string& s1, const std::string& s2) {
    const int m = s1.length();
    const int n = s2.length();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i-1] == s2[j-1])
                dp[i][j] = dp[i-1][j-1];
            else
                dp[i][j] = 1 + std::min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
        }
    }

    return dp[m][n];
}

std::string LexicalAnalyzer::findSimilarWord(const std::string& word) {
    const int MAX_DISTANCE = 2;
    std::string bestMatch = word;
    int minDistance = MAX_DISTANCE + 1;

    for (const auto& symbol : symbolTable) {
        int distance = levenshteinDistance(word, symbol);
        if (distance <= MAX_DISTANCE && distance < minDistance) {
            minDistance = distance;
            bestMatch = symbol;
        }
    }

    return bestMatch;
}

void LexicalAnalyzer::analyze(const std::string& text) {
    std::stringstream ss(text);
    std::string word;
    int line = 1;
    int column = 1;

    while (ss >> word) {
        // Verificar caracteres inválidos
        bool hasInvalidChar = false;
        for (char c : word) {
            if (!isValidChar(c)) {
                std::cout << "Erro: Caractere inválido '" << c << "' encontrado na palavra: " << word << std::endl;
                hasInvalidChar = true;
                break;
            }
        }
        
        if (hasInvalidChar) continue;

        // Converter para minúsculas para comparação
        std::string lowerWord = word;
        std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);

        // Verificar se não é uma stopword
        if (stopwords.find(lowerWord) == stopwords.end()) {
            // Verificar similaridade com palavras existentes
            std::string similarWord = findSimilarWord(word);
            
            // Se encontrou uma palavra similar, usa ela
            if (similarWord != word) {
                std::cout << "Sugestão: '" << word << "' pode ser '" << similarWord << "'" << std::endl;
                word = similarWord;
            }

            // Adicionar à tabela de símbolos se ainda não existe
            if (std::find(symbolTable.begin(), symbolTable.end(), word) == symbolTable.end()) {
                symbolTable.push_back(word);
            }

            // Adicionar à fila de tokens
            tokenQueue.push(Token(word, "WORD", line, column));
        }

        column += word.length() + 1;
    }
}