#pragma once
#include <exception>
#include <string>

void assert(const std::string& input, const std::string& expected);

class assertion_error: public std::exception{};
