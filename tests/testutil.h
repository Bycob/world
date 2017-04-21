#pragma once

#include <iostream>
#include <stdexcept>

void parseInt(std::istream & input, int &value) {
    std::string line;
    std::getline(input, line);
    try {
        value = std::stoi(line);
    }
    catch (std::exception & e) {}
}

void parseDouble(std::istream & input, double &value) {
    std::string line;
    std::getline(input, line);
    try {
        value = std::stod(line);
    }
    catch (std::exception & e) {}
}