#include <iostream>
#include "LexicalAnalyzer.h"

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
    LexicalAnalyzer analyzer;

    // Teste 1: Texto simples
    std::cout << "Teste 1: Texto simples" << std::endl;
    analyzer.analyze("Ola, como voce esta hoje?");
    printAnalysisResults(analyzer);

    // Teste 2: Texto com erros de grafia
    std::cout << "\nTeste 2: Texto com erros de grafia" << std::endl;
    analyzer.analyze("Excecao Esceção excessão");
    printAnalysisResults(analyzer);

    // Teste 3: Texto com caracteres inválidos
    std::cout << "\nTeste 3: Texto com caracteres invalidos" << std::endl;
    analyzer.analyze("Hello world! §¬£");
    printAnalysisResults(analyzer);

    // Teste 4: Texto com maior caso de teste
    std::cout << "\nTeste 4: Texto com caracteres variados" << std::endl;
    analyzer.analyze("std::cout << ""olá, como. o dia está ?hoje?!44aa!, tudo asasas bem44 444aa a4a aa44 aa44ff90 aa44!hjh");
    printAnalysisResults(analyzer);

    return 0;
}