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
    int TokenPos;

    Token(std::string Type, std::string Value, int Position) {
        this->TokenType = Type;
        this->TokenValue = Value;
        this->TokenPos = Position;
    }

    bool operator== (Token other) {
        return this->TokenPos == other.TokenPos &&
               this->TokenType.compare(other.TokenType) == 0 &&
               this->TokenValue.compare(other.TokenValue) == 0;
    }

    bool operator!= (Token other) {
        return !(*this == other);
    }

    static std::vector<Token> LexicalAnalysis(std::string str) {
        std::vector<Token> tokens = std::vector<Token>();
        std::vector<Token> remove = std::vector<Token>();
        static const std::regex string_reg("\"(.*?)\"");
        static const std::regex integer("-?[0-9]+");
        static const std::regex variable("[a-zA-Z_][a-zA-Z_0-9]*(?=\\s)");
        static const std::regex assign("(?:[^<>!])([+\\-*/]?=)");
        static const std::regex compare("[<>=!]=");
        static const std::regex end_statement(";");
        static const std::regex key_word(
            "(?:[^a-zA-Z_0-9]|)(PRINT|FOR|ENDFOR)(?:[^a-zA-Z_0-9]|)");

        stealTokens(&tokens, getTokens(str, string_reg, "STR", &stringString));
        stealTokens(&tokens, getTokens(str, integer, "INT", &intString));
        stealTokens(&tokens, getTokens(str, variable, "VAR", &varString));
        stealTokens(&tokens, getTokens(str, assign, "ASSIGN", &assignString));
        stealTokens(&tokens, getTokens(str, compare, "CMP", &cmpString));
        stealTokens(&tokens, getTokens(str, end_statement, "END", &endString));

        for (Token token : tokens) {
            if (token.TokenType.compare("VAR") == 0 &&
                std::regex_search(token.TokenValue, key_word)) {
                    token.TokenType = "KEY";
            } else if (isInToken(token, tokens)) {
                remove.push_back(token);
            }
        }

        for (Token token : remove) {
            auto it = std::find(tokens.begin(), tokens.end(), token);
            if (it != tokens.end()) {
                tokens.erase(it);
            }
        }

        return tokens;
    }

 private:
    static bool isInToken(Token token, std::vector<Token> vec) {
        for (Token token2 : vec) {
            if (token.TokenPos >= token2.TokenPos &&
                token.TokenPos+token.TokenValue.size()-1
                    < token2.TokenPos+token2.TokenValue.size() &&
                token != token2) {
                    return true;
                }
        }
        return false;
    }
    static void stealTokens(std::vector<Token>* dest, std::vector<Token> src) {
        dest->insert(
            dest->end(),
            std::make_move_iterator(src.begin()),
            std::make_move_iterator(src.end()));
        return;
    }

    static std::vector<Token> getTokens(
            std::string str,
            std::regex ptrn,
            std::string Type,
            std::string (*func)(std::smatch)) {
        std::vector<Token> tokens = std::vector<Token>();
        std::smatch str_result;

        for (std::sregex_iterator i(str.begin(), str.end(), ptrn);
                i != std::sregex_iterator();
                i++) {
            tokens.push_back(Token(Type, func(*i), i->position()));
        }

        return tokens;
    }

    static std::string stringString(std::smatch match) {
        return match.str(1);
    }
    static std::string intString(std::smatch match) {
        return match.str();
    }
    static std::string varString(std::smatch match) {
        return match.str();
    }
    static std::string assignString(std::smatch match) {
        return match.str(1);
    }
    static std::string cmpString(std::smatch match) {
        return match.str();
    }
    static std::string endString(std::smatch match) {
        return ";";
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Incorrect number of arguments (needs a file to run).\n";
        return 0;
    } else if (argc > 2) {
        std::cout <<
            "Incorrect number of arguments (only executes one file).\n";
        return 0;
    }
    std::ifstream file;
    try {
        file = std::ifstream(argv[1]);
    } catch (std::exception e) {
        file.close();
        std::cerr << e.what() << std::endl;
        return 0;
    }

    std::string str = slurp(file);
    std::vector<Token> tokens = Token::LexicalAnalysis(str);
    for (Token token : tokens) {
        std::cout << "("
                  << token.TokenType
                  << ","
                  << token.TokenValue
                  << ","
                  << token.TokenPos
                  << "-"
                  << token.TokenPos+token.TokenValue.size()-1
                  << ")"
                  << std::endl;
    }
    file.close();
    return 0;
}

std::string slurp(std::ifstream& in) {
    std::ostringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}
