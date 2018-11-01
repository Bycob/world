#ifndef WORLD_WORLDCOLLECTOR_H
#define WORLD_WORLDCOLLECTOR_H

#include "core/WorldConfig.h"

#include <vector>
#include <map>

#include "WorldTypes.h"
#include "assets/Object3D.h"
#include "assets/Material.h"
#include "assets/Scene.h"

#include "ICollector.h"

namespace world {

template <typename T> class CollectorChannel;

/** Default implementation of the ICollector interface.
 * A Collector can store multiple channels and give access
 * to them. This implementation has specific methods to
 * manipulate channels of type CollectorChannel<T>. */
class WORLDAPI_EXPORT Collector : public ICollector {
public:
    Collector();

    /** Delete all the resources harvested from the previous
     * "collect" calls */
    virtual void reset();

    template <typename T> CollectorChannel<T> &addStorageChannel();

    // TODO simplify method call (only one required template argument instead of
    // 2)
    template <typename T, typename CustomChannel, typename... Args>
    CustomChannel &addCustomChannel(Args... args);

    template <typename T> bool hasStorageChannel() const;

    template <typename T> CollectorChannel<T> &getStorageChannel();

    void fillScene(Scene &scene);

protected:
#ifdef _MSC_VER
    std::map<size_t, std::shared_ptr<ICollectorChannelBase>> _channels;
#else
    std::map<size_t, std::unique_ptr<ICollectorChannelBase>> _channels;
#endif


    ICollectorChannelBase &getChannelByType(size_t type) override;

    bool hasChannelByType(size_t type) const override;
};


template <typename T> class CollectorChannelIterator;


/** This class is an implementation of ICollectorChannel<T>
 * that allows the user to store objects into a channel, and
 * to retrieve these objects with the help of an iterator.
 * @tparam T - is the type of the objects that this channel
 * can store. */
template <typename T> class CollectorChannel : public ICollectorChannel<T> {
public:
    ~CollectorChannel() override = default;

    CollectorChannel();

    CollectorChannel(const CollectorChannel<T> &other) = delete;

    void put(const ItemKey &key, const T &item) override;

    bool has(const ItemKey &key) const override;

    void remove(const ItemKey &key) override;

    const T &get(const ItemKey &key) const;


    /** Delete all the resources harvested from the previous
     * "collect" calls */
    void reset() override;

    CollectorChannelIterator<T> begin();

    CollectorChannelIterator<T> end();

protected:
#ifdef _MSC_VER
    std::map<ItemKey, std::shared_ptr<T>> _items;
#else
    std::map<ItemKey, std::unique_ptr<T>> _items;
#endif
};

template <typename T> struct CollectorEntry {
    ItemKey _key;
    const T &_value;

    CollectorEntry(const ItemKey &key, const T &value)
            : _key(key), _value(value) {}
    CollectorEntry(const CollectorEntry<T> &other)
            : _key(other._key), _value(other._value) {}
};

/** An iterator to iterate over a CollectorChannel<T>.
 * @tparam T  */
template <typename T> class CollectorChannelIterator {
public:
    // TODO The iterator does not work well with std methods yet.
    /*using iterator_category = std::forward_iterator_tag;
    using value_type = CollectorEntry<T>;
    using difference_type = std::ptrdiff_t;
    using pointer = CollectorEntry<T>*;
    using reference = CollectorEntry<T>&;*/


#ifdef _MSC_VER
    CollectorChannelIterator(
        typename std::map<ItemKey, std::shared_ptr<T>>::iterator it);
#else
    CollectorChannelIterator(
        typename std::map<ItemKey, std::unique_ptr<T>>::iterator it);
#endif

    CollectorChannelIterator<T> &operator++();

    CollectorEntry<T> operator*() const;

    bool operator==(const CollectorChannelIterator<T> &other) const;

    bool operator!=(const CollectorChannelIterator<T> &other) const;

private:
#ifdef _MSC_VER
    typename std::map<ItemKey, std::shared_ptr<T>>::iterator _it;
#else
    typename std::map<ItemKey, std::unique_ptr<T>>::iterator _it;
#endif
};

} // namespace world

#include "Collector.inl"

#endif // WORLD_WORLDCOLLECTOR_H
