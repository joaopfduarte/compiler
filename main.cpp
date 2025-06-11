#include <iostream>
#include "LexicalAnalyzer/LexicalAnalyzer.h"
#include "SyntaxAnalyzer/SyntaxAnalyzer.h"

void printAnalysisResults(const LexicalAnalyzer &analyzer) {
    std::cout << "\nTabela de Simbolos:" << std::endl;
    for (const auto &symbol: analyzer.getSymbolTable()) {
        std::cout << symbol << std::endl;
    }

    std::cout << "\nFila de Tokens:" << std::endl;
    auto tokenQueue = analyzer.getTokenQueue();
    while (!tokenQueue.empty()) {
        const auto &token = tokenQueue.front();
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

        /*
         * Cleaning of queue and tree before next iteration
         */

        syntaxAnalyzer.~SyntaxAnalyzer();
        new(&syntaxAnalyzer) SyntaxAnalyzer();

        /*
                 Análise léxica
        */
        lexAnalyzer.analyze(input);
        auto tokens = lexAnalyzer.getTokenQueue();
        std::cout << "\n[DEBUG] Tokens gerados pela análise léxica:" << std::endl;
        auto tokenCopy = tokens;
        while (!tokenCopy.empty()) {
            const auto& token = tokenCopy.front();
            std::cout << "Lexema: '" << token.lexeme
                      << "', Tipo: '" << token.type
                      << "', Linha: " << token.line
                      << ", Coluna: " << token.column << std::endl;
            tokenCopy.pop();
        }

        /*
         Análise sintática
        */
        ParsedQuery result = syntaxAnalyzer.analyze(tokens);

        if (!result.isComplete) {
            std::string missing = syntaxAnalyzer.getMissingElement();
            if (!missing.empty()) {
                std::cout << "Tipo da consulta: " << static_cast<int>(result.type) << std::endl;
                std::cout << "Elemento faltante: " << missing << std::endl;
                std::cout << "Qual " << missing << " você deseja saber?" << std::endl;
                std::getline(std::cin, input);

                lexAnalyzer.analyze(input);
                tokens = lexAnalyzer.getTokenQueue();
                if (syntaxAnalyzer.handleResponse(tokens)) {
                    std::cout << "Consulta completada com sucesso!\n";
                } else {
                    std::cout << "Não foi possível completar a consulta.\n";
                }
            }
        } else {
            std::cout << "Consulta reconhecida com sucesso!\n";
            for (const auto &param: result.parameters) {
                std::cout << param.first << ": " << param.second << std::endl;
            }

            std::string response = syntaxAnalyzer.generateResponse(result);
            std::cout << "Resposta: " << response << std::endl;

        }
    }

    return 0;
}
