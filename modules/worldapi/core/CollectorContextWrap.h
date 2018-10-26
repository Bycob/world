#ifndef WORLD_COLLECTORCONTEXTWRAP_H
#define WORLD_COLLECTORCONTEXTWRAP_H

#include "core/WorldConfig.h"

#include <map>
#include <utility>
#include <memory>

#include "math/Vector.h"
#include "ICollector.h"

namespace world {

class ICollectorContext {
public:
    virtual ~ICollectorContext() = default;

    virtual ItemKey mutateKey(const ItemKey &key) const = 0;

    virtual vec3d getOffset() const = 0;
};

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

	const T &get(const ItemKey &key) const override;

	std::string keyToString(const ItemKey &key) const override;

private:
	ICollectorContext &_context;
	ICollectorChannel<T> &_wrapped;
};

} // namespace world

#include "CollectorContextWrap.inl"

#endif // WORLD_COLLECTORCONTEXTWRAP_H
