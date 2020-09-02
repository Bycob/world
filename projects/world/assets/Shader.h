#pragma once

#include "world/core/WorldConfig.h"

#include <string>
#include <vector>

#include "world/core/WorldFile.h"

namespace world {

struct WORLDAPI_EXPORT ShaderParam {
    enum class Type { NONE, SCALAR, INTEGER, TEXTURE, TEXTURE_ARRAY };

    static ShaderParam NONE;
    static const int MAX_SIZE = 256;

    Type _type;
    std::string _name;
    std::string _value;

    void getData(void *data, int &size) const;
};

class WORLDAPI_EXPORT Shader : public ISerializable {
public:
    Shader(std::string vertexPath, std::string fragmentPath)
            : _vertexPath{std::move(vertexPath)}, _fragmentPath{std::move(
                                                      fragmentPath)} {}

    void setVertexPath(std::string vertexPath);

    void setFragmentPath(std::string fragmentPath);

    std::string getVertexPath() const { return _vertexPath; }

    std::string getFragmentPath() const { return _fragmentPath; }

    void addParameter(ShaderParam param);

    const auto &getParameters() const { return _params; }

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

private:
    std::string _vertexPath;
    std::string _fragmentPath;

    std::vector<ShaderParam> _params;
};
} // namespace world
