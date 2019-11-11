#pragma once

#include "world/core/WorldConfig.h"

#include <string>

namespace world {

struct WORLDAPI_EXPORT ShaderParam {
    enum class Type { NONE, SCALAR, INTEGER, TEXTURE, TEXTURE_ARRAY };

    static ShaderParam NONE;

    Type _type;
    std::string _value;
};

class WORLDAPI_EXPORT Shader {
public:
    Shader(std::string vertexPath, std::string fragmentPath)
            : _vertexPath{std::move(vertexPath)}, _fragmentPath{std::move(
                                                      fragmentPath)} {}

    void setVertexPath(std::string vertexPath) {
        _vertexPath = std::move(vertexPath);
    }

    void setFragmentPath(std::string fragmentPath) {
        _fragmentPath = std::move(fragmentPath);
    }

    std::string getVertexPath() const { return _vertexPath; }

    std::string getFragmentPath() const { return _fragmentPath; }

private:
    std::string _vertexPath;
    std::string _fragmentPath;
};
} // namespace world
