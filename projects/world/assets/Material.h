#pragma once

#include "world/core/WorldConfig.h"

#include <string>
#include <map>

#include "Color.h"
#include "Shader.h"

namespace world {

class WORLDAPI_EXPORT Material {
public:
    Material();

    Material(const std::string &name);

    virtual ~Material();

    void setName(const std::string &name);

    std::string getName() const { return _name; }

    void setShader(std::string shader);

    std::string getShader() const { return _shader; }

    void setShaderParam(std::string name, const ShaderParam &value);

    ShaderParam getShaderParam(const std::string &name) const;

    const auto &getShaderParams() const { return _shaderParams; }

    void setKd(double r, double g, double b);

    Color4d getKd() const { return _Kd; }

    void setKa(double r, double g, double b);

    Color4d getKa() const { return _Ka; }

    void setKs(double r, double g, double b);

    Color4d getKs() const { return _Ks; }

    void setMapKd(const std::string &texName);

    std::string getMapKd() const { return _mapKd; }

    void setTransparent(bool transparent) { _transparent = transparent; }

    bool isTransparent() const { return _transparent; }

private:
    std::string _name;
    std::string _shader;

    std::map<std::string, ShaderParam> _shaderParams;

    /// Specular exponent from 0 to 1000
    double _Ns;
    /// Refraction coefficient
    double _Ni;
    Color4d _Kd;
    Color4d _Ka;
    Color4d _Ks;
    std::string _mapKd;
    std::string _mapKs;
    std::string _mapBump;

    bool _transparent = false;
};
} // namespace world
