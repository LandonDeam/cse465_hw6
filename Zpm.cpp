#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>

int main(int argc, char** argv) {


}

class Token {
  public:
    std::string TokenType;
    std::string TokenValue;

private:
    static const std::regex string("(['\"`])([^\1]*?)((?<!\\)\1)");
    static const std::regex integer("-?[0-9]+");
    static const std::regex variable("[a-zA-Z_][a-zA-Z_0-9]*(?=\s|;|\n)");
    static const std::regex assign("(?<![<>])[+\-*\/]?=");
    static const std::regex compare("(?<![+\-*\/])[<>=!]=");
    static const std::regex end_statement(";");

public:
    Token(std::string Type, std::string Value) {
        this->TokenType = Type;
        this->TokenValue = Value;
    }

    static std::vector<Token> LexicalAnalysis(std::string str) {
        std::vector<Token> tokens = std::vector<Token>();

        return tokens;
        // string: (['"`])([^\1]*?)((?<!\\)\1)
        // integer: -?[0-9]+
        // variable: [a-zA-Z_][a-zA-Z_0-9]*(?=\s|;|\n)
        // assignment: (?<![<>])[+\-*\/]?=
        // compare: (?<![+\-*\/])[<>=!]=
        // end statement: ;
    }
};

