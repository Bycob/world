//
// Created by louis on 23/04/17.
//

#ifndef WORLD_INITFILE_H
#define WORLD_INITFILE_H

#include <string>
#include <map>

class IniFile {
public:
    static IniFile read(const std::string & filename);

    IniFile();
    IniFile(const std::string & content);

    std::string getValue(const std::string & key) const;
    std::string getOrSetValue(const std::string & key, const std::string & defaultValue);

private:
    std::map<std::string, std::string> _values;

    void parse(const std::string & content);
};


#endif //WORLD_INITFILE_H
