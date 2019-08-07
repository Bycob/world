#pragma once

#include "world/core/WorldConfig.h"

#include <string>

namespace world {

struct WORLDAPI_EXPORT ShaderParam {
    enum class Type {
        NONE,
        SCALAR,
        INTEGER,
        TEXTURE,
        TEXTURE_ARRAY
    };

    static ShaderParam NONE;

    Type _type;
    std::string _value;
};

class WORLDAPI_EXPORT Shader {
public:
    //TODO

};
} // namespace world
