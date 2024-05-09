// => I’m competing for BONUS Points <=
// Copyright Landon Deam 2024

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <unordered_map>
#include <memory>
#include <variant>

std::string slurp(std::ifstream& in);
template<typename T>
static void addVec(std::vector<T>* dest, std::vector<T> src);

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

        addVec(&tokens, getTokens(str, integer, &intString));
        addVec(&tokens, getTokens(str, variable, &varString));
        addVec(&tokens, getTokens(str, assign, &assignString));
        addVec(&tokens, getTokens(str, compare, &cmpString));
        addVec(&tokens, getTokens(str, end_statement, &endString));
        addVec(&tokens, getTokens(str, string_reg, &stringString));

        for (Token token : tokens) {
            if (token.isInVector(tokens)) {
                remove.push_back(token);
            }
        }

        for (Token token : remove) {
            auto it = std::find(tokens.begin(), tokens.end(), token);
            if (it != tokens.end()) {
                tokens.erase(it);
            }
        }

        quicksort(&tokens, 0, tokens.size()-1);

        return tokens;
    }

 private:
    static std::regex string_reg;
    static std::regex integer;
    static std::regex variable;
    static std::regex assign;
    static std::regex compare;
    static std::regex end_statement;

    bool isInVector(std::vector<Token> vec) {
        for (Token token : vec) {
            if (this->TokenPos >= token.TokenPos &&
                this->TokenPos+this->TokenValue.size()
                    <= token.TokenPos+token.TokenValue.size() &&
                *this != token) {
                    return true;
                }
        }
        return false;
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
        return s == "PRINT" ||
               s == "FOR" ||
               s == "ENDFOR";
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

    static void quicksort(std::vector<Token>* vec, int L, int R) {
        int i, j, mid;
        Token piv;
        i = L;
        j = R;
        mid = L + (R - L) / 2;
        piv = (*vec)[mid];

        while (i<R || j>L) {
            while ((*vec)[i] < piv)
                i++;
            while ((*vec)[j] > piv)
                j--;

            if (i <= j) {
                swap((*vec), i, j);
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

class Memory {
 public:
    std::string type;
    std::variant<int, std::string> data;

    static void assignMem(std::string location, std::string type, Memory data) {
        if (type == "=") {
            mem[location] = data;
        } else if (type == "+=" ||
                type == "-=" ||
                type == "*=" ||
                type == "/=") {
            if (mem[location].type != data.type) {
                // Handle error: mismatched types
                return;
            }
            // Perform operation based on the type
            if (mem[location].type == "INT") {
                // Add, subtract, multiply, or divide integers
                if (std::holds_alternative<int>(mem[location].data) &&
                    std::holds_alternative<int>(data.data)) {
                    int arg1 = std::get<int>(mem[location].data);
                    int arg2 = std::get<int>(data.data);
                    if (type == "+=") {
                        arg1 += arg2;
                    } else if (type == "-=") {
                        arg1 -= arg2;
                    } else if (type == "*=") {
                        arg1 *= arg2;
                    } else if (type == "/=") {
                        if (arg2 == 0) {
                            // Handle division by zero error
                        } else {
                            arg1 /= arg2;
                        }
                    }
                    mem[location].data = arg1;
                } else {
                    // Handle error: invalid data type
                }
            } else if (mem[location].type == "STR" && type == "+=") {
                // Concatenate strings
                if (std::holds_alternative<std::string>(mem[location].data) &&
                    std::holds_alternative<std::string>(data.data)) {
                    std::string& arg1 =
                        std::get<std::string>(mem[location].data);
                    const std::string& arg2 = std::get<std::string>(data.data);
                    arg1 += arg2;
                } else {
                    // Handle error: invalid data type
                }
            } else {
                // Handle error: unsupported operation
            }
        } else {
            // Handle error: invalid operation
        }
    }


    static Memory readMem(std::string location) {
        if (isNotInMemory(location)) {
            // TODO(LandonDeam) throw runtime error;
        }
        return mem[location];
    }

    static int readInt(std::string location) {
        if (isNotInMemory(location)) {
            // TODO(LandonDeam) throw runtime error;
            return 0;
        }
        if (mem[location].type == "INT") {
            return std::get<int>(mem[location].data);
        } else {
            return 0;
        }
    }

    static std::string toString(std::string key) {
        if (isNotInMemory(key)) {
            // TODO(LandonDeam) throw runtime error;
        }
        if (mem[key].type == "STR") {
            return std::get<std::string>(mem[key].data);
        } else if (mem[key].type == "INT") {
            return std::to_string(
                std::get<int>(mem[key].data));
        }
        return "";
    }

    static void print(std::string key) {
        std::cout << key << "=" << toString(key) << std::endl;
    }

    static void initMem() {
        mem = std::unordered_map<std::string, Memory>();
    }

    static bool isNotInMemory(std::string key) {
        return mem.find(key) == mem.end();
    }

    Memory(std::string t,
           std::variant<int, std::string> d)
           : type(t), data(d) {}

    Memory() = default;

 private:
    static std::unordered_map<std::string, Memory> mem;
};

class Statement {
 public:
    std::vector<Token> tokens;

    static std::vector<Statement> parse(std::vector<Token> tokens) {
        std::vector<Statement> out = std::vector<Statement>();
        for (int i = 0; i < tokens.size(); i++) {
            if (tokens.at(i).TokenType == "KEY" &&
                tokens.at(i).TokenValue == "FOR") {
                int posEndFor = -1;
                int fors = 1;
                for (int j = i+2; j < tokens.size(); j++) {
                    if (tokens.at(j).TokenType == "KEY") {
                        if (tokens.at(j).TokenValue == "FOR") {
                            fors++;
                        } else if (
                            tokens.at(j).TokenValue == "ENDFOR") {
                            if (fors <= 1) {
                                posEndFor = j;
                                break;
                            } else {
                                fors--;
                            }
                        }
                    }
                }
                out.push_back(Statement(std::vector<Token>(
                    tokens.begin()+i,
                    tokens.begin()+posEndFor+1)));
                i = posEndFor;
            } else if (tokens.at(i).TokenType == "VAR") {
                out.push_back(Statement(
                    std::vector<Token>(
                        tokens.begin()+i,
                        tokens.begin()+i+4)));
                i+=3;
            } else if (tokens.at(i).TokenType == "KEY" &&
                       tokens.at(i).TokenValue == "PRINT") {
                out.push_back(Statement(
                    std::vector<Token>(
                        tokens.begin()+i,
                        tokens.begin()+i+3)));
                i+=2;
            }
        }
        return out;
    }

    explicit Statement(std::vector<Token> t) {
        this->tokens = t;
    }

    void execute() {
        if (tokens.at(0).TokenType == "KEY") {
            if (tokens.at(0).TokenValue == "PRINT") {
                Memory::print(tokens.at(1).TokenValue);
            } else if (tokens.at(0).TokenValue == "FOR") {
                for (int j = 0;
                    j < ((tokens.at(1).TokenType == "VAR") ?
                        Memory::readInt(tokens.at(1).TokenValue) :
                        std::stoi(tokens.at(1).TokenValue));
                    j++) {
                    std::vector<Statement> statements =
                    parse(std::vector<Token>(tokens.begin()+2,
                                             tokens.end()));
                    for (int k = 0; k < statements.size(); k++) {
                        statements.at(k).execute();
                    }
                }
            }
        } else if (tokens.at(0).TokenType == "VAR") {
            if (tokens.at(2).TokenType == "VAR") {
                Memory::assignMem(
                    tokens.at(0).TokenValue,
                    tokens.at(1).TokenValue,
                    Memory::readMem(tokens.at(2).TokenValue));
            } else {
                std::variant<int, std::string> value;
                if (tokens.at(2).TokenType == "STR") {
                    value = tokens.at(2).TokenValue;
                } else if (tokens.at(2).TokenType == "INT") {
                    value = std::stoi(tokens.at(2).TokenValue);
                }
                Memory::assignMem(
                    tokens.at(0).TokenValue,
                    tokens.at(1).TokenValue,
                    Memory(tokens.at(2).TokenType, value));
            }
        }
    }
};

std::unordered_map<std::string, Memory> Memory::mem;

std::regex Token::string_reg("\"(.*?)\"");
std::regex Token::integer("-?[0-9]+");
std::regex Token::variable("[a-zA-Z_][a-zA-Z_0-9]*(?=\\s)");
std::regex Token::assign("(?:[^<>!])([+\\-*/]?=)");
std::regex Token::compare("[<>=!]=");
std::regex Token::end_statement(";");

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
    Memory::initMem();
    std::vector<Statement> statements = Statement::parse(tokens);
    for (int i = 0; i < statements.size(); i++) {
        statements.at(i).execute();
    }
    file.close();
    return 0;
}

std::string slurp(std::ifstream& in) {
    std::ostringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}

template<typename T>
static void addVec(std::vector<T>* dest, std::vector<T> src) {
    dest->insert(
        dest->end(),
        std::make_move_iterator(src.begin()),
        std::make_move_iterator(src.end()));
    return;
}