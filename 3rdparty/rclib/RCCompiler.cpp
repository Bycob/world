#include <fstream>
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char** argv) {
    if (argc < 2) {
        return -1;
    }

    std::ofstream dest(argv[1]);
    // TODO check if stream is good

    for (int i = 2; i < argc; ++i) {
        // Read file
        std::string path(argv[i]);
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        if (!file.good()) {
            throw std::runtime_error("File not found " + path);
        }

        size_t filesize = static_cast<size_t>(file.tellg());
        std::vector<char> content(filesize);
        file.seekg(0, file.beg);
        file.read(content.data(), filesize);
        file.close();

        // Write header
        std::string filename = path.substr(path.find_last_of("/\\") + 1);
        // std::string delim = "rclib_include";
        dest << "{\"" << filename << "\",{";

        for (char c : content) {
            int val = int(c);
            dest << val << ",";
        }

        dest << "}}," << std::endl;
    }

    dest.close();
    return 0;
}