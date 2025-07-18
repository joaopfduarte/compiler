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

void testTranslator(Translator &translator) {
    SyntaxTreeNode *root = new SyntaxTreeNode(Token{"root", "ROOT", 0, 0});

    SyntaxTreeNode *queryNode = new SyntaxTreeNode(Token{"query", "QUERY", 1, 1});
    root->addChild(queryNode);

    SyntaxTreeNode *termNode = new SyntaxTreeNode(Token{"documentos", "TERM", 1, 3});
    queryNode->addChild(termNode);

    SyntaxTreeNode *andNode = new SyntaxTreeNode(Token{"AND", "OPERATOR", 1, 5});
    SyntaxTreeNode *nextTermNode = new SyntaxTreeNode(Token{"compiladores", "TERM", 1, 7});
    andNode->addChild(nextTermNode);
    queryNode->addChild(andNode);

    std::string result = translator.translateToIEEEQuery(root);

    std::cout << "[TESTE] URL gerada pelo documento:\n" << result << "\n";

    delete root;
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
        } else if (result.isComplete) {
            std::cout << "Consulta reconhecida com sucesso!\n";
            for (const auto &param: result.parameters) {
                std::cout << param.first << ": " << param.second << std::endl;
            }

            SyntaxTreeNode *syntaxTreeRoot = syntaxAnalyzer.getSyntaxTreeRoot();
            std::string ieeQueryUrl = translator.translateToIEEEQuery(syntaxTreeRoot);
            std::cout << "IEEE Query URL: " << ieeQueryUrl << std::endl;

            if (ieeQueryUrl == "URL_CORRETA_ESPERADA") {
                std::cout << "URL gerada corretamente!\n";
            } else {
                std::cout << "URL gerada não corresponde ao esperado!\n";
            }
        } else
            std::cout << "Consulta inválida!\n";
    }

    return 0;
}
