#ifndef WORLD_WORLDFILE_H
#define WORLD_WORLDFILE_H

#include "world/core/WorldConfig.h"

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

#include "WorldTypes.h"
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

    void addDouble(const std::string &id, double f);

    double readDouble(const std::string &id) const;

    bool readDoubleOpt(const std::string &id, double &d) const;

    void addFloat(const std::string &id, float f);

    float readFloat(const std::string &id) const;

    bool readFloatOpt(const std::string &id, float &f) const;

    void addInt(const std::string &id, int i);

    int readInt(const std::string &id) const;

    bool readIntOpt(const std::string &id, int &i) const;

    void addUint(const std::string &id, u32 u);

    u32 readUint(const std::string &id) const;

    bool readUintOpt(const std::string &id, u32 &u) const;

    void addBool(const std::string &id, bool b);

    bool readBool(const std::string &id) const;

    bool readBoolOpt(const std::string &id, bool &b) const;

    template <typename T> void addStruct(const std::string &id, const T &s);

    template <typename T> void readStruct(const std::string &id, T &s) const;

    void addArray(const std::string &id);

    void addToArray(const std::string &id, WorldFile &&item);

    WorldFileIterator readArray(const std::string &id) const;

    void addChild(const std::string &id, WorldFile &&child);

    const WorldFile &readChild(const std::string &id) const;

    // IO

    void save(const std::string &filename) const;

    void load(const std::string &filename);

    std::string toJson() const;

    void fromJson(const std::string &jsonStr);

private:
    std::shared_ptr<Json> _jdoc;

    std::map<std::string, std::unique_ptr<WorldFile>> _children;
    std::map<std::string, std::vector<std::unique_ptr<WorldFile>>> _arrays;


    void writeToJdoc(Json &json, rapidjson::Value &jval) const;

    void readFromJdoc(const rapidjson::Value &jval);

    friend class WorldFileIterator;
};

template <typename T> void write(const T &s, WorldFile &wf);

template <typename T> void read(const WorldFile &wf, T &s);

template <typename T> inline WorldFile serialize(const T &s) {
    WorldFile wf;
    world::write<T>(s, wf);
    return wf;
}

template <typename T> inline T deserialize(const WorldFile &wf) {
    T t;
    world::read<T>(wf, t);
    return t;
}

template <typename T>
inline void WorldFile::addStruct(const std::string &id, const T &s) {
    addChild(id, serialize(s));
}

template <typename T>
inline void WorldFile::readStruct(const std::string &id, T &s) const {
    world::read<T>(*this, s);
}

class WORLDAPI_EXPORT WorldFileIterator {
public:
    void operator++();

    const WorldFile &operator*() const;

    const WorldFile *operator->() const;

    bool end() const;

private:
    typedef std::vector<std::unique_ptr<WorldFile>>::const_iterator
        iterator_type;

    iterator_type _current;
    iterator_type _end;

    WorldFileIterator(const std::vector<std::unique_ptr<WorldFile>> &vec);

    friend class WorldFile;
};

class WORLDAPI_EXPORT ISerializable {
public:
    virtual ~ISerializable() = default;

    WorldFile serialize() const {
        WorldFile f;
        write(f);
        return f;
    }

    WorldFile serializeSubclass() const {
        WorldFile f;
        writeSubclass(f);
        return f;
    }

    virtual void writeSubclass(WorldFile &file) const;

    void save(const std::string &filename) const;

    virtual void write(WorldFile &wf) const {};

    void load(const std::string &filename);

    virtual void read(const WorldFile &wf) {}
};

template <typename T>
std::map<std::string, std::function<T *(const WorldFile &file)>>
    &getDeserializeIndex();

template <typename T> T *readSubclass(const WorldFile &file);

#define WORLD_REGISTER_BASE_CLASS(ClassName)                                   \
    template <>                                                                \
    std::map<std::string, std::function<ClassName *(const WorldFile &file)>>   \
        &getDeserializeIndex<ClassName>() {                                    \
        static std::map<std::string,                                           \
                        std::function<ClassName *(const WorldFile &file)>>     \
            index;                                                             \
        return index;                                                          \
    }                                                                          \
                                                                               \
    template <> ClassName *readSubclass<ClassName>(const WorldFile &file) {    \
        std::string type = file.readString("type");                            \
        auto &deserIndex = getDeserializeIndex<ClassName>();                   \
        auto readFunc = deserIndex.at(type);                                   \
        return readFunc(file);                                                 \
    }

#define WORLD_SECOND_REGISTER_CHILD_CLASS(ParentClass, ChildClass, ClassID)    \
    ParentClass *read##ChildClass(const WorldFile &wf) {                       \
        auto *instance = new ChildClass();                                     \
        instance->read(wf);                                                    \
        return instance;                                                       \
    }                                                                          \
                                                                               \
    void *_register##ChildClass##_##ParentClass = []() {                       \
        auto &deserIndex = getDeserializeIndex<ParentClass>();                 \
        deserIndex[ClassID] = read##ChildClass;                                \
        return nullptr;                                                        \
    }();

#define WORLD_REGISTER_CHILD_CLASS(ParentClass, ChildClass, ClassID)           \
    ParentClass *read##ChildClass(const WorldFile &wf) {                       \
        auto *instance = new ChildClass();                                     \
        instance->read(wf);                                                    \
        return instance;                                                       \
    }                                                                          \
                                                                               \
    void *_register##ChildClass##_##ParentClass = []() {                       \
        auto &deserIndex = getDeserializeIndex<ParentClass>();                 \
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

#include "Serialization.inl"

#endif // WORLD_WORLDFILE_H
