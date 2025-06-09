#ifndef SYNTAXANALYZER_H
#define SYNTAXANALYZER_H

#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <algorithm>
#include "../Token.h"

enum class QueryType {
    FORMAT_QUERY,
    SIZE_QUERY,
    TITLE_QUERY,
    DATE_QUERY,
    KEYWORD_QUERY,
    RESPONSE,
    UNKNOWN
};

struct ParsedQuery {
    QueryType type;
    std::map<std::string, std::string> parameters;
    bool isComplete;
    std::string missingElement;
};
class SyntaxAnalyzer {
private:
    std::vector<std::string> validFormats = {
        ".pdf", ".docx", ".xlsx", ".pptx", ".txt", ".rtf", ".odt",
        ".ods", ".odp", ".csv", ".html", ".htm", ".xml", ".json",
        ".zip", ".rar", ".7z", ".tar", ".gz", ".mp3", ".jpg",
        ".png", ".mp4"
    };

    ParsedQuery currentQuery;
    std::vector<ParsedQuery> queryHistory;

    bool isValidFormat(const std::string& format);
    bool isValidDate(const std::string& date);
    QueryType identifyQueryType(const std::queue<Token>& tokenQueue);
    void analyzeFormatQuery(std::queue<Token> tokens);
    void analyzeSizeQuery(std::queue<Token> tokens);
    void analyzeTitleQuery(std::queue<Token> tokens);
    void analyzeDateQuery(std::queue<Token> tokens);
    void analyzeKeywordQuery(std::queue<Token> tokens);
    void analyzeResponse(std::queue<Token> tokens);

public:
    SyntaxAnalyzer() {}

    ParsedQuery analyze(std::queue<Token> tokens);
    std::string getMissingElement() const;
    bool handleResponse(std::queue<Token> tokens);

};



#endif //SYNTAXANALYZER_H
