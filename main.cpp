#include <iostream>
#include "LexicalAnalyzer/LexicalAnalyzer.h"
#include "SyntaxAnalyzer/SyntaxAnalyzer.h"
#include "Translator/Translator.h"
#include "SyntaxAnalyzer/SyntaxTreeNode.h"

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
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::cout << std::unitbuf;
    std::locale::global(std::locale(""));


    LexicalAnalyzer lexAnalyzer;
    SyntaxAnalyzer syntaxAnalyzer;
    Translator translator;
    std::string input;

    while (true) {
        std::cout << "\nDigite sua pergunta (ou 'sair' para encerrar): ";
        std::getline(std::cin, input);

        if (input == "sair") break;

        /*
         * Cleaning of queue and tree before next iteration
         */

        /*syntaxAnalyzer.~SyntaxAnalyzer();
        new(&syntaxAnalyzer) SyntaxAnalyzer();*/
        lexAnalyzer.~LexicalAnalyzer();
        new(&lexAnalyzer) LexicalAnalyzer();

        /*
                 Análise léxica
        */
        lexAnalyzer.analyze(input);
        auto tokens = lexAnalyzer.getTokenQueue();
        std::cout << "\n[DEBUG] Tokens gerados pela análise léxica:" << std::endl;
        auto tokenCopy = tokens;
        while (!tokenCopy.empty()) {
            const auto &token = tokenCopy.front();
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
                std::cout << "[INFO] Tipo da consulta: " << static_cast<int>(result.type) << std::endl;
                std::cout << "[INFO] Elemento faltante detectado: " << missing << std::endl;
                std::cout << "[INPUT] Qual " << missing << " você deseja usar? ";
                std::getline(std::cin, input);

                lexAnalyzer.analyze(input);
                tokens = lexAnalyzer.getTokenQueue();

                if (syntaxAnalyzer.handleResponse(tokens)) {
                    std::cout << "[SUCCESS] Consulta completada com sucesso!\n";

                    SyntaxTreeNode *root = syntaxAnalyzer.getSyntaxTreeRoot();
                    std::string ieeeQueryUrl = translator.translateToIEEEQuery(root, result.type);
                    std::cout << "[RESULT] URL para o IEEE: " << ieeeQueryUrl << '\n';
                } else {
                    std::cout << "[ERROR] Não foi possível completar a consulta.\n";
                }
            } else {
                std::cout << "[ERROR] Não foi possível identificar o elemento faltante.\n";
            }
        } else if (result.isComplete) {
            std::cout << "[INFO] Consulta reconhecida com sucesso!\n";
            for (const auto &param : result.parameters) {
                std::cout << param.first << ": " << param.second << '\n';
            }

            SyntaxTreeNode *root = syntaxAnalyzer.getSyntaxTreeRoot();
            std::string ieeeQueryUrl = translator.translateToIEEEQuery(root, result.type);
            std::cout << "[RESULT] URL para o IEEE: " << ieeeQueryUrl << '\n';

            if (ieeeQueryUrl == "URL_CORRETA_ESPERADA") {
                std::cout << "[SUCCESS] URL gerada corretamente!\n";
            } else {
                std::cout << "[WARNING] A URL gerada não corresponde com o esperado.\n";
            }
        } else {
            std::cout << "[ERROR] Consulta inválida ou não suportada!\n";
        }
    }

    return 0;
}
