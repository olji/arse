#include <iostream>
#include <string>

#include "testing.hpp"

void assert(const std::string& input, const std::string& expected){
    if(input != expected){
        std::cout << "Expected: \"" << expected << "\"" << std::endl;
        std::cout << "Actual: \"" << input << "\"" << std::endl;
        throw assertion_error();
    }
}
