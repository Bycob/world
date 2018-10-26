#ifndef WORLD_ICOLLECTOR_H
#define WORLD_ICOLLECTOR_H

#include "core/WorldConfig.h"

#include <tuple>

#include "WorldKeys.h"

namespace world {

class ICollectorChannelBase;
template <typename T> class ICollectorChannel;

/***/
class ICollector {
public:
    template <typename T> ICollectorChannel<T> &getChannel();

    template <typename T> bool hasChannel() const;

protected:
    virtual ICollectorChannelBase &getChannelByType(size_t type) = 0;

    virtual bool hasChannelByType(size_t type) const = 0;

    /*void passItemTo(ICollector &collector, const ItemKey &key,
                    Object3D &object) {
        collector.addItemUnsafe(key, object);
    }

    virtual void addItemUnsafe(const ItemKey &key, Object3D &object) {
        addItem(key, object);
    }*/


    // TODO find another solution
    friend class CollectorContextWrap;
};


class ICollectorContext;


class ICollectorChannelBase {
public:
	virtual ~ICollectorChannelBase() = default;

	virtual ICollectorChannelBase *wrap(ICollectorContext &context) = 0;
};


template <typename T> class ICollectorChannel : public ICollectorChannelBase {
public:
	ICollectorChannelBase *wrap(ICollectorContext &context) override;

	virtual void put(const ItemKey &key, const T &item) = 0;

	virtual bool has(const ItemKey &key) const = 0;

	virtual void remove(const ItemKey &key) = 0;

	virtual const T &get(const ItemKey &key) const = 0;

	/** Converts a key to a string that can be used as texture name /
	* material name / other to reference the object having this key
	* in this channel. */
	virtual std::string keyToString(const ItemKey &key) const = 0;
};



template <typename T> inline ICollectorChannel<T> &ICollector::getChannel() {
    size_t type = typeid(ICollectorChannel<T>).hash_code();
    return dynamic_cast<ICollectorChannel<T> &>(getChannelByType(type));
}

template <typename T> inline bool ICollector::hasChannel() const {
    size_t type = typeid(ICollectorChannel<T>).hash_code();
    return hasChannelByType(type);
}

}; // namespace world

#include "CollectorContextWrap.h"
#endif // WORLD_ICOLLECTOR_H
