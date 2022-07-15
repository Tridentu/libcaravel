
#include "backend.h"
#include <iostream>
#include <vector>

namespace CaravelPM::Script {
    Token::Token(CaravelPM::Script::TokenType newType, std::any newValue)
    {
        Type = newType;
        Value = newValue;
    }


    Interpreter::Interpreter(std::string text)
    {
        m_Text = text;
        position = 0;
    }
    
    void Interpreter::error()
    {
        throw "Error parsing input";
    }

    
    Interpreter::~Interpreter()
    {
        
    }
    
    void Interpreter::advance(int increment)
    {
        position += increment;
    }

    
    Token* Interpreter::getIntToken(char currentChar, const std::string& currentText)
    {
        char currentCharInt = currentChar;
        std::vector<char> charVec;
        while(isdigit(currentCharInt)){
            if(position > m_Text.size() - 1)
                break;
            currentCharInt = currentText.at(position);
            if(isdigit(currentCharInt)){
                charVec.push_back(currentCharInt);
                advance();
            } else {
                    break;

            }
        }
        std::string intString = "";
        for (auto charV : charVec){
            intString += charV;
        }
        Token* token = new Token(TokenType::Integer, std::stoi(intString));
        return token;
    }


    Token* Interpreter::getNextToken()
    {
        try {
            std::string currentText = m_Text;
            if(position > m_Text.size() - 1){
                return new Token(TokenType::EndOfFile, nullptr);
            }
            
            char currentChar = currentText.at(position);
            if (isdigit(currentChar)){
                Token* token = getIntToken(currentChar, currentText);
                return token;
            }
            if(currentChar == '+'){
                Token* token = new Token(TokenType::Plus, currentChar);
                position++;
                return token;
            }
            else if(currentChar == '-'){
                Token* token = new Token(TokenType::Minus, currentChar);
                advance();
                return token;
            }
            else if(currentChar == '*'){
                Token* token = new Token(TokenType::Multiply, currentChar);
                advance();
                return token;
            }
            else if(currentChar == '/'){
                Token* token = new Token(TokenType::Divide, currentChar);
                advance();
                return token;
            }
            if (isspace((int)currentChar)){
                advance();
                return new Token(TokenType::Whitespace, currentChar);
            }
            error();
        } catch (std::exception err){
            return new Token(TokenType::Error, nullptr);
        } 
    }
    
    void Interpreter::setText(const std::string& text)
    {
        m_Text = text;
    }
    
    Token* Interpreter::eatInt()
    {
        Token* token = m_currentToken;
        eat(TokenType::Integer);
        return token;
    }

    bool Interpreter::isArithmetic()
    {
        return m_currentToken->Type == TokenType::Plus || m_currentToken->Type == TokenType::Minus || m_currentToken->Type == TokenType::Multiply || m_currentToken->Type == TokenType::Divide;
    }


    
    int Interpreter::expr()
    {
        int result;
        m_currentToken = getNextToken();
        result = TokenValueTo<int>(eatInt());
        while(isArithmetic()){
            Token* token = m_currentToken;
            if(token->Type == TokenType::Plus) {
                eat(TokenType::Plus);
                result += TokenValueTo<int>(eatInt());
            }else if(token->Type == TokenType::Multiply){
                eat(TokenType::Multiply);
                result *= TokenValueTo<int>(eatInt());
            }else if (token->Type == TokenType::Divide){
                eat(TokenType::Divide);
                result /= TokenValueTo<int>(eatInt());
            }else if (token->Type == TokenType::Minus){
                eat(TokenType::Minus);
                result -= TokenValueTo<int>(eatInt());
            }
        }
        return result;
    }
    
    void Interpreter::eat(TokenType tType)
    {
        try {
            if(getNextToken()->Type == TokenType::Whitespace){
                while(m_currentToken->Type == TokenType::Whitespace){
                    m_currentToken = getNextToken();
                }
            }
           
            if(m_currentToken->Type == tType){
                m_currentToken = getNextToken();
            } else {
               std::cout << "Expected " << (int)tType << ", got " << (int)m_currentToken->Type << "( " << position << ")"  << std::endl;
                error();
            }
        } catch(std::exception ex) {
            std::cout << "Error encountered: " << ex.what() << std::endl;
        }
    }


}
