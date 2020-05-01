#include "WorldFile.h"

#include <utility>
#include <iostream>

// https://rapidjson.org/md_doc_tutorial.html

namespace world {

using namespace rapidjson;

WorldFile::WorldFile() : _jdoc(std::make_shared<Json>(kObjectType)) {}

WorldFile::WorldFile(WorldFile &&wf)
        : _jdoc(wf._jdoc), _children(std::move(wf._children)),
          _arrays(std::move(wf._arrays)) {}

WorldFile &WorldFile::operator=(WorldFile &&wf) {
    _jdoc = wf._jdoc;
    return *this;
}

void WorldFile::addString(const std::string &id, const std::string &str) {
    _jdoc->AddMember(JsonUtils::strToVal(id, *_jdoc),
                     JsonUtils::strToVal(str, *_jdoc), _jdoc->GetAllocator());
}

std::string WorldFile::readString(const std::string &id) const {
    if (!_jdoc->HasMember(id))
        throw std::runtime_error("WorldFile: No member named " + id);
    if (!(*_jdoc)[id].IsString())
        throw std::runtime_error("WorldFile: " + id + " not of type 'String'");
    return std::string((*_jdoc)[id].GetString(),
                       (*_jdoc)[id].GetStringLength());
}

bool WorldFile::readStringOpt(const std::string &id, std::string &str) const {
    if (_jdoc->HasMember(id) && (*_jdoc)[id].IsString()) {
        str = std::string((*_jdoc)[id].GetString(),
                          (*_jdoc)[id].GetStringLength());
        return true;
    } else
        return false;
}

void WorldFile::addDouble(const std::string &id, double f) {
    _jdoc->AddMember(JsonUtils::strToVal(id, *_jdoc), Value().SetDouble(f),
                     _jdoc->GetAllocator());
}

double WorldFile::readDouble(const std::string &id) const {
    if (!_jdoc->HasMember(id))
        throw std::runtime_error("WorldFile: No member named " + id);
    if (!(*_jdoc)[id].IsDouble())
        throw std::runtime_error("WorldFile: " + id + " not of type 'Double'");
    return (*_jdoc)[id].GetDouble();
}

bool WorldFile::readDoubleOpt(const std::string &id, double &d) const {
    if (_jdoc->HasMember(id) && (*_jdoc)[id].IsDouble()) {
        d = (*_jdoc)[id].GetDouble();
        return true;
    } else
        return false;
}

void WorldFile::addFloat(const std::string &id, float f) {
    _jdoc->AddMember(JsonUtils::strToVal(id, *_jdoc), Value().SetFloat(f),
                     _jdoc->GetAllocator());
}

float WorldFile::readFloat(const std::string &id) const {
    if (!_jdoc->HasMember(id))
        throw std::runtime_error("WorldFile: No member named " + id);
    if (!(*_jdoc)[id].IsFloat())
        throw std::runtime_error("WorldFile: " + id + " not of type 'Float'");
    return (*_jdoc)[id].GetFloat();
}

bool WorldFile::readFloatOpt(const std::string &id, float &f) const {
    if (_jdoc->HasMember(id) && (*_jdoc)[id].IsFloat()) {
        f = (*_jdoc)[id].GetFloat();
        return true;
    } else
        return false;
}

void WorldFile::addInt(const std::string &id, int i) {
    _jdoc->AddMember(JsonUtils::strToVal(id, *_jdoc), Value().SetInt(i),
                     _jdoc->GetAllocator());
}

int WorldFile::readInt(const std::string &id) const {
    if (!_jdoc->HasMember(id))
        throw std::runtime_error("WorldFile: No member named " + id);
    if (!(*_jdoc)[id].IsInt())
        throw std::runtime_error("WorldFile: " + id + " not of type 'Int'");
    return (*_jdoc)[id].GetInt();
}

bool WorldFile::readIntOpt(const std::string &id, int &i) const {
    if (_jdoc->HasMember(id) && (*_jdoc)[id].IsInt()) {
        i = (*_jdoc)[id].GetInt();
        return true;
    } else
        return false;
}

void WorldFile::addUint(const std::string &id, world::u32 u) {
    _jdoc->AddMember(JsonUtils::strToVal(id, *_jdoc), Value().SetUint(u),
                     _jdoc->GetAllocator());
}

u32 WorldFile::readUint(const std::string &id) const {
    if (!_jdoc->HasMember(id))
        throw std::runtime_error("WorldFile: No member named " + id);
    if (!(*_jdoc)[id].IsUint())
        throw std::runtime_error("WorldFile: " + id + " not of type 'Int'");
    return (*_jdoc)[id].GetUint();
}

bool WorldFile::readUintOpt(const std::string &id, u32 &u) const {
    if (_jdoc->HasMember(id) && (*_jdoc)[id].IsUint()) {
        u = (*_jdoc)[id].GetUint();
        return true;
    } else
        return false;
}

void WorldFile::addBool(const std::string &id, bool b) {
    _jdoc->AddMember(JsonUtils::strToVal(id, *_jdoc), Value().SetBool(b),
                     _jdoc->GetAllocator());
}

bool WorldFile::readBool(const std::string &id) const {
    if (!_jdoc->HasMember(id))
        throw std::runtime_error("WorldFile: No member named " + id);
    if (!(*_jdoc)[id].IsBool())
        throw std::runtime_error("WorldFile: " + id + " not of type 'Bool'");
    return (*_jdoc)[id].GetBool();
}

bool WorldFile::readBoolOpt(const std::string &id, bool &b) const {
    if (_jdoc->HasMember(id) && (*_jdoc)[id].IsBool()) {
        b = (*_jdoc)[id].GetBool();
        return true;
    } else
        return false;
}

void WorldFile::addArray(const std::string &id) {
    _arrays.insert(
        std::make_pair(id, std::vector<std::unique_ptr<WorldFile>>()));
}

void WorldFile::addToArray(const std::string &id, WorldFile &&item) {
    auto it = _arrays.find(id);

    if (it == _arrays.end()) {
        // The array could be created automatically without asking to the user.
        // The reason we don't do that, is because the arrays elements are
        // generally serialized in a loop. If no element is serialized, the
        // array is not created, triggering an error when deserializing. So even
        // if the array contains 0 element, you still want it to be serialized.
        throw std::runtime_error("Please create the array with addArray() "
                                 "before adding any item to it");
    }

    auto &vec = it->second;
    auto ptr = std::make_unique<WorldFile>(std::move(item));
    vec.emplace_back(std::move(ptr));
}

WorldFileIterator WorldFile::readArray(const std::string &id) const {
    auto it = _arrays.find(id);

    if (it == _arrays.end()) {
        throw std::runtime_error("WorldFile: No array named " + id);
    }
    return WorldFileIterator(it->second);
}

void WorldFile::addChild(const std::string &id, WorldFile &&child) {
    _children.insert(
        std::make_pair(id, std::make_unique<WorldFile>(std::move(child))));
}

const WorldFile &WorldFile::readChild(const std::string &id) const {
    auto it = _children.find(id);

    if (it == _children.end()) {
        throw std::runtime_error("WorldFile: No child named " + id);
    }
    return *it->second;
}

bool WorldFile::hasChild(const std::string &id) const {
    return _children.find(id) != _children.end();
}

// TODO make toJson, fromJson, save and load methods work a consistent way

void WorldFile::save(const std::string &filename) const {
    Json jdoc;
    writeToJdoc(jdoc, jdoc);
    JsonUtils::write(filename, jdoc);
}

void WorldFile::load(const std::string &filename) {
    std::ifstream is(filename, std::ios::ate);
    size_t filesize = static_cast<size_t>(is.tellg());
    char *buf = new char[filesize];
    is.seekg(0, is.beg);
    is.read(buf, filesize);
    is.close();

    Json jdoc;
    jdoc.Parse(buf, filesize);
    readFromJdoc(jdoc);

    delete[] buf;
}

void WorldFile::fromJson(const std::string &jsonStr) {
    Json jdoc;
    jdoc.Parse(jsonStr);

    readFromJdoc(jdoc);
}

std::string WorldFile::toJson() const {
    Json jdoc;
    writeToJdoc(jdoc, jdoc);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    jdoc.Accept(writer);
    return std::string(buffer.GetString(), buffer.GetLength());
}

void WorldFile::writeToJdoc(Json &json, Value &jval) const {
    jval.CopyFrom(*_jdoc, json.GetAllocator());

    for (const auto &e : _arrays) {
        jval.AddMember(JsonUtils::strToVal(e.first, json), Value().SetArray(),
                       json.GetAllocator());

        for (auto &ptr : e.second) {
            Value val;
            ptr->writeToJdoc(json, val);
            jval[e.first].PushBack(val, _jdoc->GetAllocator());
        }
    }

    for (const auto &e : _children) {
        // TODO improve usage of the library syntax
        Value val;
        e.second->writeToJdoc(json, val);
        jval.AddMember(JsonUtils::strToVal(e.first, json), val,
                       _jdoc->GetAllocator());
    }
}

void WorldFile::readFromJdoc(const Value &jval) {
    for (auto it = jval.MemberBegin(); it != jval.MemberEnd(); it++) {
        if (it->value.IsArray()) {
            std::string id = it->name.GetString();
            _arrays[id] = std::move(std::vector<std::unique_ptr<WorldFile>>{});
            auto &vec = _arrays[id];

            for (auto it2 = it->value.Begin(); it2 != it->value.End(); it2++) {
                auto ptr = std::make_unique<WorldFile>();
                ptr->readFromJdoc(*it2);
                vec.emplace_back(std::move(ptr));
            }
        } else if (it->value.IsObject()) {
            auto ptr = std::make_unique<WorldFile>();
            ptr->readFromJdoc(it->value);
            _children[it->name.GetString()] = std::move(ptr);
        } else {
            // TODO improve syntax
            Value name;
            name.CopyFrom(it->name, _jdoc->GetAllocator());
            Value value;
            value.CopyFrom(it->value, _jdoc->GetAllocator());
            _jdoc->AddMember(name, value, _jdoc->GetAllocator());
        }
    }
}


// ##### WorldFileIterator


WorldFileIterator::WorldFileIterator(
    const std::vector<std::unique_ptr<WorldFile>> &vec)
        : _current(vec.begin()), _end(vec.end()) {}

const WorldFile &WorldFileIterator::operator*() const { return *(*_current); }

const WorldFile *WorldFileIterator::operator->() const {
    return _current->get();
}

void WorldFileIterator::operator++() { _current++; }

bool WorldFileIterator::end() const { return _current == _end; }


// ##### ISerializable


void ISerializable::load(const std::string &filename) {
    WorldFile wf;
    wf.load(filename);
    read(wf);
}

void ISerializable::writeSubclass(WorldFile &file) const {
    // references ?

    write(file);
}

void ISerializable::save(const std::string &filename) const {
    WorldFile wf;
    write(wf);
    wf.save(filename);
}
} // namespace world
