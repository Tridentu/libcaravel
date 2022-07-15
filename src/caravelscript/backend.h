
#pragma once 
#include <string>
#include <any>

namespace CaravelPM::Script {
    enum class TokenType {
        Integer,
        Plus,
        EndOfFile,
        Error,
        Whitespace,
        Minus,
        Multiply,
        Divide
    };
    
    
    struct Token {
        TokenType Type = TokenType::Integer;
        std::any Value;
        Token(TokenType newType, std::any newValue);
        Token(const Token& other) =default; 
    };
    
    
    template <typename To>
    To TokenValueTo(Token* token){
     return std::any_cast<To>(token->Value);   
    }

    class Interpreter {
    public:
        Interpreter(std::string text);
        ~Interpreter();
        Token* getCurrentToken() const;
        int getPos() const;
        int expr();
        void setText(const std::string& text);
    protected:
        Token* getIntToken(char currentChar, const std::string& currentText);
        void advance(int increment = 1);
    private:
        void error();
        Token* getNextToken();
        void eat(TokenType tType);
        Token* eatInt();
        bool isArithmetic();
    private:
        int position = -1;
        Token* m_currentToken = nullptr;
        std::string m_Text;
    };
}
