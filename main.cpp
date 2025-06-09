#include <iostream>
#include "LexicalAnalyzer/LexicalAnalyzer.h"
#include "SyntaxAnalyzer/SyntaxAnalyzer.h"

void printAnalysisResults(const LexicalAnalyzer& analyzer) {
    std::cout << "\nTabela de Simbolos:" << std::endl;
    for (const auto& symbol : analyzer.getSymbolTable()) {
        std::cout << symbol << std::endl;
    }

    std::cout << "\nFila de Tokens:" << std::endl;
    auto tokenQueue = analyzer.getTokenQueue();
    while (!tokenQueue.empty()) {
        const auto& token = tokenQueue.front();
        std::cout << "Lexema: " << token.lexeme 
                  << " (Linha: " << token.line 
                  << ", Coluna: " << token.column << ")" << std::endl;
        tokenQueue.pop();
    }
}

int main() {
    LexicalAnalyzer lexAnalyzer;
    SyntaxAnalyzer syntaxAnalyzer;
    std::string input;

    while (true) {
        std::cout << "\nDigite sua pergunta (ou 'sair' para encerrar): ";
        std::getline(std::cin, input);

        if (input == "sair") break;

        // Análise léxica
        lexAnalyzer.analyze(input);
        auto tokens = lexAnalyzer.getTokenQueue();

        // Análise sintática
        ParsedQuery result = syntaxAnalyzer.analyze(tokens);

        if (!result.isComplete) {
            std::string missing = syntaxAnalyzer.getMissingElement();
            if (!missing.empty()) {
                std::cout << "Qual " << missing << " você deseja saber?" << std::endl;
                std::getline(std::cin, input);
                lexAnalyzer.analyze(input);
                tokens = lexAnalyzer.getTokenQueue();
                syntaxAnalyzer.handleResponse(tokens);
            }
        }
    }

    return 0;
}