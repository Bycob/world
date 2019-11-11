
#ifndef WORLD_JSONUTILS_H
#define WORLD_JSONUTILS_H

#include "world/core/WorldConfig.h"

#include <fstream>

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

namespace world {
using Json = rapidjson::Document;

class JsonUtils {
public:
    static void write(const std::string &path, Json &json) {
        using namespace rapidjson;
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        json.Accept(writer);

        std::ofstream file(path);
        file << buffer.GetString();
    }
};
} // namespace world

#endif // WORLD_JSONUTILS_H
