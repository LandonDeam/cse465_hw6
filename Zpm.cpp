// Copyright Landon Deam 2024

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>

std::string slurp(std::ifstream& in);

class Token {
 public:
    std::string TokenType;
    std::string TokenValue;

    Token(std::string Type, std::string Value) {
        this->TokenType = Type;
        this->TokenValue = Value;
    }

    static std::vector<Token> LexicalAnalysis(std::string str) {
        std::vector<Token> tokens = std::vector<Token>();
        static const std::regex string_reg("(['\"`])" "([^\1]*?)""\1");
        static const std::regex integer("-?[0-9]+");
        static const std::regex variable("[a-zA-Z_][a-zA-Z_0-9]*(?=\\s|;|\n)");
        static const std::regex assign("(?<![<>])[+\\-*\\/]?=");
        static const std::regex compare("(?<![+\\-*\\/])[<>=!]=");
        static const std::regex end_statement(";");

        std::smatch str_result;
        if (std::regex_match(str, str_result, string_reg)) {
            std::smatch::iterator str_it = str_result.begin();
            for (std::advance(str_it, 1);
                 str_it != str_result.end();
                 advance(str_it, 1)) {
                std::cout << *str_it << std::endl;
            }
        }

        return tokens;
        // string: (['"`])([^\1]*?)((?<!\\)\1)
        // integer: -?[0-9]+
        // variable: [a-zA-Z_][a-zA-Z_0-9]*(?=\s|;|\n)
        // assignment: (?<![<>])[+\-*\/]?=
        // compare: (?<![+\-*\/])[<>=!]=
        // end statement: ;
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Incorrect number of arguments (needs a file to run).\n";
        return 0;
    } else if (argc > 2) {
        std::cout <<
            "Incorrect number of arugments (only executes one file).\n";
        return 0;
    }
    std::ifstream input;
    try {
        input = std::ifstream(argv[1]);
    } catch (std::exception e) {
        input.close();
        std::cerr << e.what() << std::endl;
        return 0;
    }

    Token::LexicalAnalysis(slurp(input));

    input.close();
    return 0;
}

std::string slurp(std::ifstream& in) {
    std::ostringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}
