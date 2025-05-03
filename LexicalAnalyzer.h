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