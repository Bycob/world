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

private:
    /*template <typename T>
    CollectorChannel<T> &getStorageChannel();*/
};


template <typename T> class CollectorChannelIterator;


/**
 *
 * @tparam T
 */
template <typename T> class CollectorChannel : public ICollectorChannel<T> {
public:
    ~CollectorChannel() override = default;

    CollectorChannel();

    CollectorChannel(const CollectorChannel<T> &other) = delete;

    void put(const ItemKey &key, const T &item) override;

    bool has(const ItemKey &key) const override;

    void remove(const ItemKey &key) override;

    const T &get(const ItemKey &key) const override;


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

template <typename T> class CollectorChannelIterator {
public:
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
