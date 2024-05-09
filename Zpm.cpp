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

    Token() {}

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

    bool operator> (Token other) {
        return this->TokenPos > other.TokenPos;
    }

    bool operator< (Token other) {
        return this->TokenPos < other.TokenPos;
    }

    bool operator>= (Token other) {
        return this->TokenPos >= other.TokenPos;
    }

    bool operator<= (Token other) {
        return this->TokenPos <= other.TokenPos;
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

        stealTokens(&tokens, getTokens(str, integer, &intString));
        stealTokens(&tokens, getTokens(str, variable, &varString));
        stealTokens(&tokens, getTokens(str, assign, &assignString));
        stealTokens(&tokens, getTokens(str, compare, &cmpString));
        stealTokens(&tokens, getTokens(str, end_statement, &endString));
        stealTokens(&tokens, getTokens(str, string_reg, &stringString));

        for (Token token : tokens) {
            if (isInToken(token, tokens)) {
                remove.push_back(token);
            }
        }

        for (Token token : remove) {
            auto it = std::find(tokens.begin(), tokens.end(), token);
            if (it != tokens.end()) {
                tokens.erase(it);
            }
        }

        quicksort(tokens, 0, tokens.size()-1);

        return tokens;
    }

 private:
    static bool isInToken(Token token, std::vector<Token> vec) {
        for (Token token2 : vec) {
            if (token.TokenPos >= token2.TokenPos &&
                token.TokenPos+token.TokenValue.size()
                    <= token2.TokenPos+token2.TokenValue.size() &&
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
            Token (*func)(std::smatch)) {
        std::vector<Token> tokens = std::vector<Token>();
        std::smatch str_result;

        for (std::sregex_iterator i(str.begin(), str.end(), ptrn);
                i != std::sregex_iterator();
                i++) {
            tokens.push_back(func(*i));
        }

        return tokens;
    }
    static bool isKeyWord(std::string s) {
        return s.compare("PRINT") == 0 ||
               s.compare("FOR") == 0 ||
               s.compare("ENDFOR") == 0;
    }

    static Token stringString(std::smatch match) {
        return Token("STR", match.str(1), match.position(1));
    }
    static Token intString(std::smatch match) {
        return Token("INT", match.str(), match.position());
    }
    static Token varString(std::smatch match) {
        return Token(isKeyWord(match.str()) ? "KEY" : "VAR",
                     match.str(),
                     match.position());
    }
    static Token assignString(std::smatch match) {
        return Token("ASSIGN", match.str(1), match.position(1));
    }
    static Token cmpString(std::smatch match) {
        return Token("CMP", match.str(), match.position());
    }
    static Token endString(std::smatch match) {
        return Token("END", ";", match.position());
    }

    static void quicksort(std::vector<Token> &vec, int L, int R) {
        int i, j, mid;
        Token piv;
        i = L;
        j = R;
        mid = L + (R - L) / 2;
        piv = vec[mid];

        while (i<R || j>L) {
            while (vec[i] < piv)
                i++;
            while (vec[j] > piv)
                j--;

            if (i <= j) {
                swap(vec, i, j);
                i++;
                j--;
            } else {
                if (i < R)
                    quicksort(vec, i, R);
                if (j > L)
                    quicksort(vec, L, j);
                return;
            }
        }
    }

    static void swap(std::vector<Token>& v, int x, int y) {
        Token temp = v[x];
        v[x] = v[y];
        v[y] = temp;
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
