//
// Created by louis on 23/04/17.
//

#include "IniFile.h"

#include <iostream>
#include <regex>

#include <worldapi/StringOps.h>

IniFile IniFile::read(const std::string &filename) {
    return IniFile("");
}

IniFile::IniFile() {

}

IniFile::IniFile(const std::string & content) {
    parse(content);
}

bool IniFile::hasKey(const std::string &key) const {
    return _values.find(key) != _values.end();
}

int IniFile::getKeyCount() const {
    return (int) _values.size();
}

std::string IniFile::getValue(const std::string &key) const {
    return _values.at(key);
}

std::string IniFile::getOrSetValue(const std::string &key, const std::string &defaultValue) {
    auto value = _values.find(key);

    if (value == _values.end()) {
        _values[key] = defaultValue;
        return defaultValue;
    }
    else {
        return value->second;
    }
}

void IniFile::parse(const std::string &content) {
    _values.clear();
    std::string currentSection("");
    auto lines = split(content, '\n', true);

    std::regex sectionrgx("^\\[(.*)\\]$");
    std::regex keyvalrgx("^([^=\\s]*)\\s*=\\s*([^=]*)$");

    for (std::string & line : lines) {
        std::smatch res;

        if (std::regex_match(line, res, keyvalrgx)) {
            std::string key = currentSection == "" ? res[1] : currentSection + "." + res[1].str();
            _values[key] = res[2];
        }
        else if (std::regex_match(line, res, sectionrgx)) {
            currentSection = res[1];
        }
    }
}
