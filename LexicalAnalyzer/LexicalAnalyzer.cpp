#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <algorithm>
#include "../Token.h"

class LexicalAnalyzer {
private:
    std::set<std::string> stopwords;
    std::vector<std::string> symbolTable;
    std::queue<Token> tokenQueue;

    /*
     Conjunto de caracteres válidos
*/

    std::string validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzãéçóàáÁÀèÈéÉúÚ0123456789"
            ".,;:!?()[]{}+-*/=><\"'@#$%&_\\| \t\n\r";

public:
    LexicalAnalyzer() {
        loadStopwords();
    }

    void loadStopwords();

    bool isValidChar(char c) const;

    int levenshteinDistance(const std::string &s1, const std::string &s2);

    std::string findSimilarWord(const std::string &word);

    void analyze(const std::string &text);

    const std::vector<std::string> &getSymbolTable() const { return symbolTable; }
    std::queue<Token> getTokenQueue() const { return tokenQueue; }

private:
    void processWord(const std::string &word, int line, int column);
};

#endif
#include "../LexicalAnalyzer/LexicalAnalyzer.h"
#include <fstream>
#include <sstream>
#include <iostream>

void LexicalAnalyzer::loadStopwords() {
    /*
     Fonte das stopwords: https://gist.github.com/alopes/5358189
*/

    std::vector<std::string> defaultStopwords = {
        "a", "ao", "aos", "aquela", "aquelas", "aquele", "aqueles", "aquilo",
        "as", "até", "com", "como", "da", "das", "de", "dela", "delas", "dele",
        "deles", "depois", "do", "dos", "e", "ela", "elas", "ele", "eles",
        "entre", "era", "eram", "essa", "essas", "esse", "esses", "estas",
        "este", "estes", "eu", "foi", "há", "isso", "isto", "já", "lhe",
        "lhes", "mais", "mas", "me", "mesmo", "meu", "meus", "minha", "minhas",
        "muito", "na", "nas", "nem", "nos", "nossa", "nossas", "nosso",
        "nossos", "num", "numa", "os", "ou", "para", "pela", "pelas", "pelo",
        "pelos", "por", "quando", "que", "quem", "são", "se", "seja",
        "sem", "seu", "seus", "só", "sua", "suas", "também", "te",
        "seu", "sua", "teu", "tua", "teus", "tuas", "um", "uma", "umas", "uns"
    };

    stopwords = std::set<std::string>(defaultStopwords.begin(), defaultStopwords.end());
}

bool LexicalAnalyzer::isValidChar(char c) const {
    return validChars.find(c) != std::string::npos;
}

int LexicalAnalyzer::levenshteinDistance(const std::string &s1, const std::string &s2) {
    const int m = s1.length();
    const int n = s2.length();
    std::vector<std::vector<int> > dp(m + 1, std::vector<int>(n + 1));

    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
        }
    }

    return dp[m][n];
}

std::string LexicalAnalyzer::findSimilarWord(const std::string &word) {
    const int MAX_DISTANCE = 2;
    std::string bestMatch = word;
    int minDistance = MAX_DISTANCE + 1;

    for (const auto &symbol: symbolTable) {
        int distance = levenshteinDistance(word, symbol);
        if (distance <= MAX_DISTANCE && distance < minDistance) {
            minDistance = distance;
            bestMatch = symbol;
        }
    }

    return bestMatch;
}

void LexicalAnalyzer::analyze(const std::string &text) {
    std::string currentWord;
    int line = 1;
    int column = 1;

    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];

        if (!isValidChar(c)) {
            std::cout << "Erro: Caractere invalido '" << c << "' encontrado na posicao " << i << std::endl;
            continue;
        }

        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
            c == ',' || c == '.' || c == '!' || c == '?' ||
            c == ';' || c == ':' || c == '(' || c == ')' ||
            c == '[' || c == ']' || c == '{' || c == '}') {
            if (!currentWord.empty()) {
                processWord(currentWord, line, column);
                column += currentWord.length();
                currentWord.clear();
            }

            if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
                std::string tokenType;

                if (c == ',' || c == '.' || c == '!' || c == '?' || c == ';' || c == ':') {
                    tokenType = "PUNCTUATION";
                } else if (c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}') {
                    tokenType = "DELIMITER";
                }

                tokenQueue.push(Token(std::string(1, c), tokenType, line, column));
            }


            if (c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
        } else {
            currentWord += c;
        }
    }

    if (!currentWord.empty()) {
        processWord(currentWord, line, column);
    }
}

void LexicalAnalyzer::processWord(const std::string &originalWord, int line, int column) {
    std::string word = originalWord;
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);

    if (stopwords.find(lowerWord) == stopwords.end()) {
        std::string similarWord = findSimilarWord(word);

        if (similarWord != word) {
            std::cout << "Sugestao: '" << word << "' pode ser '" << similarWord << "'" << std::endl;
            word = similarWord;
        }


        if (std::find(symbolTable.begin(), symbolTable.end(), word) == symbolTable.end()) {
            symbolTable.push_back(word);
        }

        tokenQueue.push(Token(word, "WORD", line, column));
    }
}
