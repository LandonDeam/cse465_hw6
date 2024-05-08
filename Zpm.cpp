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

    Token(std::string Type, std::string Value) {
        this->TokenType = Type;
        this->TokenValue = Value;
    }

    static Token LexicalAnalysis(std::string str) {
        return Token("","");
        // string: (['"`])([^\1]*?)(\1)
        // variable: [a-zA-Z_][a-zA-Z_0-9]+(?=\s)
    }
};

