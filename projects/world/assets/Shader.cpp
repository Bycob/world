#include "Shader.h"

namespace world {

ShaderParam ShaderParam::NONE{Type::NONE, "", ""};

void ShaderParam::getData(void *data, int &size) const {
    int id = 0;
    int start = 0;
    int stop = 0;

    int *idata = reinterpret_cast<int *>(data);
    float *fdata = reinterpret_cast<float *>(data);

    switch (_type) {
    case Type::INTEGER:
        do {
            stop = _value.find(',', start);
            idata[id] = std::stoi(_value.substr(
                start, stop == std::string::npos ? stop : stop - start));
            ++id;
            start = stop + 1;
        } while (start != std::string::npos);

        size = id * sizeof(int);
        break;
    case Type::SCALAR:
        do {
            stop = _value.find(',', start);
            fdata[id] = std::stof(_value.substr(
                start, stop == std::string::npos ? stop : stop - start));
            ++id;
            start = stop + 1;
        } while (stop != std::string::npos);

        size = id * sizeof(float);
        break;
    }
}

template <>
void WORLDAPI_EXPORT write<ShaderParam>(const ShaderParam &sp, WorldFile &wf) {
    wf.addInt("type", static_cast<int>(sp._type));
    wf.addString("name", sp._name);
    wf.addString("value", sp._value);
}

template <>
void WORLDAPI_EXPORT read<ShaderParam>(const WorldFile &wf, ShaderParam &sp) {
    int type;
    wf.readIntOpt("type", type);
    if (type < 0 || type > static_cast<int>(ShaderParam::Type::TEXTURE_ARRAY))
        throw std::runtime_error("Bad value for enum type ShaderParam::Type");
    sp._type = static_cast<ShaderParam::Type>(type);

    wf.readStringOpt("name", sp._name);
    wf.readStringOpt("value", sp._value);
}


void Shader::setVertexPath(std::string vertexPath) {
    _vertexPath = std::move(vertexPath);
}

void Shader::setFragmentPath(std::string fragmentPath) {
    _fragmentPath = std::move(fragmentPath);
}

void Shader::addParameter(ShaderParam param) {
    _params.emplace_back(std::move(param));
}

void Shader::write(WorldFile &wf) const {
    wf.addString("vertexPath", _vertexPath);
    wf.addString("fragmentPath", _fragmentPath);

    wf.addArray("params");

    for (auto &param : _params) {
        wf.addToArray("params", world::serialize(param));
    }
}

void Shader::read(const WorldFile &wf) {
    wf.readStringOpt("vertexPath", _vertexPath);
    wf.readStringOpt("fragmentPath", _fragmentPath);

    for (auto it = wf.readArray("params"); !it.end(); ++it) {
        _params.emplace_back(deserialize<ShaderParam>(*it));
    }
}

} // namespace world
