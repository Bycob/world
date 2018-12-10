#ifndef WORLD_COLLECTORCONTEXTWRAP_H
#define WORLD_COLLECTORCONTEXTWRAP_H

#include "world/core/WorldConfig.h"

#include <map>
#include <utility>
#include <memory>

#include "world/math/Vector.h"
#include "ICollector.h"

namespace world {

class ICollectorContext {
public:
    virtual ~ICollectorContext() = default;

    virtual ItemKey mutateKey(const ItemKey &key) const = 0;

    virtual vec3d getOffset() const = 0;
};

/** A CollectorContextWrap is used to wrap a collector and
 * apply some modifications to the provided data in order to
 * match a specific context. */
class CollectorContextWrap : public ICollector, ICollectorContext {
public:
    ICollector &_collector;

    CollectorContextWrap(ICollector &wrapped);

    void setCurrentChunk(ChunkKey key);

    void setCurrentObject(ObjectKey key);

    void setOffset(const vec3d &offset);

    void setKeyOffset(int keyOffset);


    ItemKey mutateKey(const ItemKey &key) const override;

    vec3d getOffset() const override;

protected:
    ICollectorChannelBase &getChannelByType(size_t type) override;

    bool hasChannelByType(size_t type) const override;

private:
#ifdef _MSC_VER
    std::map<size_t, std::shared_ptr<ICollectorChannelBase>> _wrappers;
#else
    std::map<size_t, std::unique_ptr<ICollectorChannelBase>> _wrappers;
#endif

    std::pair<bool, ChunkKey> _currentChunk;
    std::pair<bool, ObjectKey> _currentObject;
    int _keyOffset;

    vec3d _offset;
};

template <typename T>
class CollectorChannelContextWrap : public ICollectorChannel<T> {
public:
    CollectorChannelContextWrap(ICollectorContext &context,
                                ICollectorChannel<T> &wrapped);

    void put(const ItemKey &key, const T &item) override;

    bool has(const ItemKey &key) const override;

    void remove(const ItemKey &key) override;

    const T &get(const ItemKey &key) const;

private:
    ICollectorContext &_context;
    ICollectorChannel<T> &_wrapped;


    /** Changes a string to a key, then mutates it and
     * returns the string version of this key in the end.
     * If it wasn't a key at beginning, well, we don't care
     * and mutate it anyway. */
    std::string mutateKeyString(const std::string &keystr);
};

} // namespace world

#include "CollectorContextWrap.inl"

#endif // WORLD_COLLECTORCONTEXTWRAP_H
