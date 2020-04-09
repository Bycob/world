#include "WorldFile.h"

#include <utility>
#include <iostream>

// https://rapidjson.org/md_doc_tutorial.html

namespace world {

using namespace rapidjson;

WorldFile::WorldFile() : _jdoc(std::make_shared<Json>(kObjectType)) {
    _jval = _jdoc->GetObject();
}

WorldFile::WorldFile(WorldFile &&wf) : _jdoc(wf._jdoc) { _jval = wf._jval; }

WorldFile &WorldFile::operator=(WorldFile &&wf) {
    _jdoc = wf._jdoc;
    _jval = wf._jval;
    return *this;
}

void WorldFile::addString(const std::string &id, const std::string &str) {
    _jval.AddMember(JsonUtils::strToVal(id, *_jdoc),
                    JsonUtils::strToVal(str, *_jdoc), _jdoc->GetAllocator());
}

std::string WorldFile::readString(const std::string &id) const {
    if (!_jval.HasMember(id))
        throw std::runtime_error("WorldFile: No member named " + id);
    if (!_jval[id].IsString())
        throw std::runtime_error("WorldFile: " + id + " not of type 'String'");
    return std::string(_jval[id].GetString(), _jval[id].GetStringLength());
}

bool WorldFile::readStringOpt(const std::string &id, std::string &str) const {
    if (_jval.HasMember(id) && _jval[id].IsDouble()) {
        str = std::string(_jval[id].GetString(), _jval[id].GetStringLength());
        return true;
    } else
        return false;
}

void WorldFile::addFloating(const std::string &id, double f) {
    _jval.AddMember(JsonUtils::strToVal(id, *_jdoc), Value().SetDouble(f),
                    _jdoc->GetAllocator());
}

double WorldFile::readDouble(const std::string &id) const {
    if (!_jval.HasMember(id))
        throw std::runtime_error("WorldFile: No member named " + id);
    if (!_jval[id].IsDouble())
        throw std::runtime_error("WorldFile: " + id + " not of type 'Double'");
    return _jval[id].GetDouble();
}

bool WorldFile::readDoubleOpt(const std::string &id, double &d) const {
    if (_jval.HasMember(id) && _jval[id].IsDouble()) {
        d = _jval[id].GetDouble();
        return true;
    } else
        return false;
}

float WorldFile::readFloat(const std::string &id) const {
    if (!_jval.HasMember(id))
        throw std::runtime_error("WorldFile: No member named " + id);
    if (!_jval[id].IsFloat())
        throw std::runtime_error("WorldFile: " + id + " not of type 'Float'");
    return _jval[id].GetFloat();
}

bool WorldFile::readFloatOpt(const std::string &id, float &f) const {
    if (_jval.HasMember(id) && _jval[id].IsFloat()) {
        f = _jval[id].GetFloat();
        return true;
    } else
        return false;
}

void WorldFile::addInt(const std::string &id, int i) {
    _jval.AddMember(JsonUtils::strToVal(id, *_jdoc), Value().SetInt(i),
                    _jdoc->GetAllocator());
}

int WorldFile::readInt(const std::string &id) const {
    if (!_jval.HasMember(id))
        throw std::runtime_error("WorldFile: No member named " + id);
    if (!_jval[id].IsInt())
        throw std::runtime_error("WorldFile: " + id + " not of type 'Int'");
    return _jval[id].GetInt();
}

bool WorldFile::readIntOpt(const std::string &id, int &i) const {
    if (_jval.HasMember(id) && _jval[id].IsInt()) {
        i = _jval[id].GetInt();
        return true;
    } else
        return false;
}

void WorldFile::addBool(const std::string &id, bool b) {
    _jval.AddMember(JsonUtils::strToVal(id, *_jdoc), Value().SetBool(b),
                    _jdoc->GetAllocator());
}

bool WorldFile::readBool(const std::string &id) const {
    if (!_jval.HasMember(id))
        throw std::runtime_error("WorldFile: No member named " + id);
    if (!_jval[id].IsBool())
        throw std::runtime_error("WorldFile: " + id + " not of type 'Bool'");
    return _jval[id].GetBool();
}

bool WorldFile::readBoolOpt(const std::string &id, bool &b) const {
    if (_jval.HasMember(id) && _jval[id].IsBool()) {
        b = _jval[id].GetBool();
        return true;
    } else
        return false;
}

void WorldFile::addToArray(const std::string &id, WorldFile &item) {
    if (!_jval.HasMember(id)) {
        _jval.AddMember(JsonUtils::strToVal(id, *_jdoc), Value().SetArray(),
                        _jdoc->GetAllocator());
    }

    _jval[id].PushBack(Value().CopyFrom(item._jval, _jdoc->GetAllocator()),
                       _jdoc->GetAllocator());
}

void WorldFile::addToArray(const std::string &id, WorldFile &&item) {
    if (!_jval.HasMember(id)) {
        _jval.AddMember(JsonUtils::strToVal(id, *_jdoc), Value().SetArray(),
                        _jdoc->GetAllocator());
    }

    _jval[id].PushBack(Value().CopyFrom(item._jval, _jdoc->GetAllocator()),
                       _jdoc->GetAllocator());
}

WorldFileIterator &WorldFile::readArray(const std::string &id) const {
    auto res =
        _arrays.insert({id, std::unique_ptr<WorldFileIterator>(nullptr)});
    if (res.second) {
        if (!_jval.HasMember(id))
            throw std::runtime_error("WorldFile: No member named " + id);
        if (!_jval[id].IsArray())
            throw std::runtime_error("WorldFile: " + id +
                                     " not of type 'Array'");
        res.first->second.reset(
            new WorldFileIterator(_jdoc, const_cast<Value &>(_jval[id])));
    }
    return *res.first->second;
}

void WorldFile::addChild(const std::string &id, WorldFile &child) {
    _jval.AddMember(JsonUtils::strToVal(id, *_jdoc), child._jval,
                    _jdoc->GetAllocator());
}

void WorldFile::addChild(const std::string &id, WorldFile &&child) {
    _jval.AddMember(JsonUtils::strToVal(id, *_jdoc), child._jval,
                    _jdoc->GetAllocator());
}

const WorldFile &WorldFile::readChild(const std::string &id) const {
    auto res = _children.insert({id, std::unique_ptr<WorldFile>(nullptr)});
    if (res.second) {
        if (!_jval.HasMember(id))
            throw std::runtime_error("WorldFile: No member named " + id);
        if (!_jval[id].IsObject())
            throw std::runtime_error("WorldFile: " + id +
                                     " not of type 'Object'");
        res.first->second.reset(
            new WorldFile(_jdoc, const_cast<Value &>(_jval[id])));
    }
    return *res.first->second;
}

void WorldFile::save(const std::string &filename) const {
    JsonUtils::write(filename, _jval);
}

void WorldFile::load(const std::string &filename) {
    std::ifstream is(filename, std::ios::ate);
    size_t filesize = static_cast<size_t>(is.tellg());
    char *buf = new char[filesize];
    is.seekg(0, is.beg);
    is.read(buf, filesize);
    is.close();

    _jdoc = std::make_shared<Json>();
    _jdoc->Parse(buf, filesize);
    _jval = _jdoc->GetObject();

    delete[] buf;
}

void WorldFile::fromJson(const std::string &jsonStr) {
    _jdoc = std::make_shared<Json>();
    _jdoc->Parse(jsonStr);
    _jval = _jdoc->GetObject();
}

std::string WorldFile::toJson() const {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    _jval.Accept(writer);
    return std::string(buffer.GetString(), buffer.GetLength());
}

WorldFile::WorldFile(std::shared_ptr<Json> jdoc, Value &jval)
        : _jdoc(std::move(jdoc)) {
    _jval = jval;
}


// ##### WorldFileIterator


WorldFileIterator::WorldFileIterator(std::shared_ptr<Json> jdoc, Value &val)
        : _it(val.Begin()), _end(val.End()), _jdoc(std::move(jdoc)) {

    _items.push_back(std::unique_ptr<WorldFile>(new WorldFile(_jdoc, *_it)));
}

const WorldFile &WorldFileIterator::operator*() const { return *_items.back(); }

const WorldFile *WorldFileIterator::operator->() const {
    return _items.back().get();
}

void WorldFileIterator::operator++() {
    ++_it;
    _items.push_back(std::unique_ptr<WorldFile>(new WorldFile(_jdoc, *_it)));
}

bool WorldFileIterator::end() const { return _it == _end; }


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
