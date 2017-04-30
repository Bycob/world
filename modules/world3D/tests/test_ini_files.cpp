#include <iostream>
#include <regex>

#include "../../../tests/testutil.h"

#include "../IniFile.h"

int main(int argc, char** argv) {
    test([] {
        IniFile init("");
    }, "parse empty file without exception");

    test([] {
        IniFile init("");
        assertFail([&init] {init.getValue("key");});
    }, "test get unkown key fails");

    test([] {
        IniFile init("key = value");
        assert(init.getValue("key") == "value");
    }, "parse simple basic file");

    test([] {
        IniFile init("key = value\nkey2 = value2\n\nkey3 = value3");
        assert(init.getKeyCount() == 3, "key count");
        assert(init.getValue("key") == "value", "key1");
        assert(init.getValue("key2") == "value2", "key2");
        assert(init.getValue("key3") == "value3", "key3");
    }, "parse multiline file with some empty lines");

    test([] {
        IniFile init("key= value");
        assert(init.getValue("key") == "value", "key= value");

        IniFile init2("key =value");
        assert(init.getValue("key") == "value", "key =value");

        IniFile init3("key=value");
        assert(init.getValue("key") == "value", "key=value");
    }, "parse key/value advanced");

    test([] {
        IniFile init("[section]\nkey = value");
        assertFail([&init]{init.getValue("key");});
        assert(init.getValue("section.key") == "value");
    }, "parse sections : test if section name is used in key");
}

