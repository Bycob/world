#ifndef WORLD_WORLDFILE_H
#define WORLD_WORLDFILE_H

#include "world/core/WorldConfig.h"

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "JsonUtils.h"

namespace world {

class WorldFileIterator;

class WORLDAPI_EXPORT WorldFile {
public:
    WorldFile();

    WorldFile(const WorldFile &wf) = delete;

    WorldFile(WorldFile &&wf);

    WorldFile &operator=(const WorldFile &wf) = delete;

    WorldFile &operator=(WorldFile &&wf);

    void addString(const std::string &id, const std::string &str);

    std::string readString(const std::string &id) const;

    bool readStringOpt(const std::string &id, std::string &str) const;

    void addFloating(const std::string &id, double f);

    double readDouble(const std::string &id) const;

    bool readDoubleOpt(const std::string &id, double &d) const;

    float readFloat(const std::string &id) const;

    bool readFloatOpt(const std::string &id, float &f) const;

    void addInt(const std::string &id, int i);

    int readInt(const std::string &id) const;

    bool readIntOpt(const std::string &id, int &i) const;

    void addBool(const std::string &id, bool b);

    bool readBool(const std::string &id) const;

    bool readBoolOpt(const std::string &id, bool &b) const;

    void addToArray(const std::string &id, WorldFile &item);

    WorldFileIterator &readArray(const std::string &id) const;

    void addChild(const std::string &id, WorldFile &child);

    const WorldFile &readChild(const std::string &id) const;

    // IO

    void write(const std::string &filename) const;

    void read(const std::string &filename);

    std::string toJson() const;

    void fromJson(const std::string &jsonStr);

private:
    std::shared_ptr<Json> _jdoc;

    rapidjson::Value _jval;

    mutable std::map<std::string, std::unique_ptr<WorldFile>> _children;
    mutable std::map<std::string, std::unique_ptr<WorldFileIterator>> _arrays;


    WorldFile(std::shared_ptr<Json> jdoc, rapidjson::Value &value);

    friend class WorldFileIterator;
};


class WORLDAPI_EXPORT WorldFileIterator {
public:
    void operator++();

    const WorldFile &operator*() const;

    const WorldFile *operator->() const;

    bool end() const;

private:
    rapidjson::Document::ValueIterator _it;
    rapidjson::Document::ValueIterator _end;
    std::shared_ptr<Json> _jdoc;

    mutable std::vector<std::unique_ptr<WorldFile>> _items;


    WorldFileIterator(std::shared_ptr<Json> jdoc, rapidjson::Value &val);

    friend class WorldFile;
};

class WORLDAPI_EXPORT ISerializable {
public:
    virtual ~ISerializable() = default;

    WorldFile write() const {
        WorldFile f;
        write(f);
        return f;
    }

    WorldFile writeSubclass() const  {
        WorldFile f;
        writeSubclass(f);
        return f;
    }

    virtual void writeSubclass(WorldFile &file) const;

    void write(const std::string &filename) const;

    virtual void write(WorldFile &worldFile) const {};

    void read(const std::string &filename);

    virtual void read(const WorldFile &worldFile) {}
};

#define WORLD_REGISTER_BASE_CLASS(ClassName)                                   \
public:                                                                        \
    static std::map<std::string,                                               \
                    std::function<ClassName *(const WorldFile &file)>>         \
        &getDeserializeIndex() {                                               \
        static std::map<std::string,                                           \
                        std::function<ClassName *(const WorldFile &file)>>     \
            index;                                                             \
        return index;                                                          \
    }                                                                          \
                                                                               \
    static ClassName *readSubclass(const WorldFile &file) {                    \
        std::string type = file.readString("type");                            \
        auto &deserIndex = getDeserializeIndex();                              \
        auto readFunc = deserIndex.at(type);                                   \
        return readFunc(file);                                                 \
    }

#define WORLD_REGISTER_CHILD_CLASS(ParentClass, ChildClass, ClassID)           \
    ParentClass *read##ChildClass(const WorldFile &wf) {                       \
        auto *instance = new ChildClass();                                     \
        instance->read(wf);                                                    \
        return instance;                                                       \
    }                                                                          \
                                                                               \
    void *_register##ChildClass = []() {                                       \
        auto &deserIndex = ParentClass::getDeserializeIndex();                 \
        deserIndex[ClassID] = read##ChildClass;                                \
        return nullptr;                                                        \
    }();                                                                       \
                                                                               \
    void ChildClass::writeSubclass(WorldFile &wf) const {                      \
        wf.addString("type", ClassID);                                         \
        this->write(wf);                                                       \
    }

#define WORLD_WRITE_SUBCLASS_METHOD                                            \
public:                                                                        \
    void writeSubclass(WorldFile &wf) const override;

} // namespace world

#endif // WORLD_WORLDFILE_H
