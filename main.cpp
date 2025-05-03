#include <iostream>
#include "LexicalAnalyzer.h"

void printAnalysisResults(const LexicalAnalyzer& analyzer) {
    std::cout << "\nTabela de Símbolos:" << std::endl;
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
    LexicalAnalyzer analyzer;

    // Teste 1: Texto simples
    std::cout << "Teste 1: Texto simples" << std::endl;
    analyzer.analyze("Olá, como você está hoje?");
    printAnalysisResults(analyzer);

    // Teste 2: Texto com erros de grafia
    std::cout << "\nTeste 2: Texto com erros de grafia" << std::endl;
    analyzer.analyze("Excecao Esceção excessão");
    printAnalysisResults(analyzer);

    // Teste 3: Texto com caracteres inválidos
    std::cout << "\nTeste 3: Texto com caracteres inválidos" << std::endl;
    analyzer.analyze("Hello world! §¬£");
    printAnalysisResults(analyzer);

    return 0;
}