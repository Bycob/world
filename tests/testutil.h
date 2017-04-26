#pragma once

#include <iostream>
#include <stdexcept>
#include <functional>

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

// Tests unitaires

class TestFailure {
public:
    TestFailure(const std::string & failureDesc) : _failureDesc(failureDesc) {}

    const std::string & getDesc() const {
        return _failureDesc;
    }
private:
    std::string _failureDesc;
};

void test(const std::function<void()> & test, const std::string & testName,
          std::ostream & stream = std::cout) {
    try {
        test();
        stream << "\033[32m+ Test passed : " << testName << "\033[0m" << std::endl;
    }
    catch (const TestFailure & e) {
        stream << "\033[31m- Test failed : " << testName << "\033[0m" << std::endl;
        if (e.getDesc() != "") stream << "Reason : " << e.getDesc() << std::endl;
    }
    catch (std::exception & e) {
        stream << "\033[31m- Test raised an exception : " << testName << "\033[0m" << std::endl;
        stream << e.what() << std::endl;
    }
}

void assert(bool result, const std::string & assertion = "") {
    if (!result) throw TestFailure(assertion);
}

void assertFail(const std::function<void()> & function, const std::string & whatifitfails = "") {
    try {
        function();
        throw TestFailure(std::string("function did not fail : ") + whatifitfails );
    }
    catch (std::exception & e) {
        //success
    }
}