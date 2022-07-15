#include "backend.h"
#include <iostream>

int main(int argc, char** argv){
    std::string inputStr;
    while(true) {
     std::cout << std::flush;
     std::cout << "Input: " << std::endl;
     std::getline(std::cin, inputStr);
     if(inputStr == "exit")
        break;
     CaravelPM::Script::Interpreter* interp = new CaravelPM::Script::Interpreter(inputStr);
     try {
        int result = interp->expr();
         std::cout <<  result << std::endl;
     } catch(std::exception ex){
        std::cout << "An error occured." << std::endl;   
     }
     delete interp;
     inputStr = "";

    }
    return 0;
}
