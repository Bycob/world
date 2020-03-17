#ifndef WORLD_WORLDFILE_H
#define WORLD_WORLDFILE_H

#include "world/core/WorldConfig.h"

#include <string>
#include <vector>
#include <memory>

#include "JsonUtils.h"

namespace world {

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

    void addArray(const std::string &id, const std::vector<WorldFile> &array);

    void addToArray(const std::string &id, const WorldFile &item);

    std::vector<WorldFile> readArray(const std::string &id) const;

    bool readArrayOpt(const std::string &id,
                      std::vector<WorldFile> &array) const;

    void addChild(const std::string &id, const WorldFile &child);

    WorldFile readChild(const std::string &id) const;

    // IO

    void write(const std::string &filename) const;

    void read(const std::string &filename);

    std::string toJson() const;

    void fromJson(const std::string &jsonStr);

private:
    std::shared_ptr<Json> _jdoc;

    rapidjson::Value _jval;


    WorldFile(std::shared_ptr<Json> jdoc, rapidjson::Value &&value);
};

class WORLDAPI_EXPORT ISerializable {
public:
    virtual ~ISerializable() = default;

    WorldFile write() const {
        WorldFile f;
        write(f);
        return f;
    }

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
    }();

} // namespace world

#endif // WORLD_WORLDFILE_H
