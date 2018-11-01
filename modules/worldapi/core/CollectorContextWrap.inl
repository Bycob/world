#include "CollectorContextWrap.h"

#include "assets/Object3D.h"
#include "assets/Material.h"

namespace world {

template <typename T>
inline ICollectorChannelBase *ICollectorChannel<T>::wrap(
    ICollectorContext &context) {
    return new CollectorChannelContextWrap<T>(context, *this);
}

inline CollectorContextWrap::CollectorContextWrap(ICollector &wrapped)
        : _collector(wrapped), _currentChunk(false, ChunkKeys::none()),
          _currentObject(false, ObjectKeys::defaultKey()), _keyOffset(0),
          _offset() {}

inline void CollectorContextWrap::setCurrentChunk(ChunkKey key) {
    _currentChunk = std::make_pair(true, key);
}

inline void CollectorContextWrap::setCurrentObject(ObjectKey key) {
    _currentObject = std::make_pair(true, key);
}

inline void CollectorContextWrap::setOffset(const vec3d &offset) {
    _offset = offset;
}

inline void CollectorContextWrap::setKeyOffset(int keyOffset) {
    _keyOffset = keyOffset;
}

inline ICollectorChannelBase &CollectorContextWrap::getChannelByType(
    size_t type) {
    auto it = _wrappers.find(type);

    if (it == _wrappers.end()) {
#ifdef _MSC_VER
        _wrappers.emplace(type,
                          std::shared_ptr<ICollectorChannelBase>(
                              _collector.getChannelByType(type).wrap(*this)));
#else
        _wrappers.emplace(type,
                          std::unique_ptr<ICollectorChannelBase>(
                              _collector.getChannelByType(type).wrap(*this)));
#endif
        return *_wrappers[type];
    } else {
        return *it->second;
    }
}

inline bool CollectorContextWrap::hasChannelByType(size_t type) const {
    return _collector.hasChannelByType(type);
}

inline ItemKey CollectorContextWrap::mutateKey(const ItemKey &key) const {
    return ItemKeys::inWorld(
        _currentChunk.first ? _currentChunk.second : std::get<0>(key),
        _currentObject.first ? _currentObject.second : std::get<1>(key),
        std::get<2>(key) + _keyOffset);
}

inline vec3d CollectorContextWrap::getOffset() const { return _offset; }

// ======

template <typename T>
inline CollectorChannelContextWrap<T>::CollectorChannelContextWrap(
    ICollectorContext &context, ICollectorChannel<T> &wrapped)
        : _context(context), _wrapped(wrapped) {}

template <typename T>
inline void CollectorChannelContextWrap<T>::put(const ItemKey &key,
                                                const T &item) {
    _wrapped.put(_context.mutateKey(key), item);
}

template <>
inline void CollectorChannelContextWrap<Object3D>::put(const ItemKey &key,
                                                       const Object3D &item) {
    Object3D newItem(item);
    newItem.setMaterialID(mutateKeyString(newItem.getMaterialID()));
    newItem.setPosition(item.getPosition() + _context.getOffset());
    _wrapped.put(_context.mutateKey(key), newItem);
}

template <>
inline void CollectorChannelContextWrap<Material>::put(const ItemKey &key,
                                                       const Material &item) {

    Material newMaterial(item);
    newMaterial.setMapKd(mutateKeyString(item.getMapKd()));
    _wrapped.put(_context.mutateKey(key), newMaterial);
}

template <typename T>
inline bool world::CollectorChannelContextWrap<T>::has(
    const ItemKey &key) const {
    return _wrapped.has(_context.mutateKey(key));
}

template <typename T>
inline void CollectorChannelContextWrap<T>::remove(const ItemKey &key) {
    _wrapped.remove(_context.mutateKey(key));
}

template <typename T>
inline const T &CollectorChannelContextWrap<T>::get(const ItemKey &key) const {
    return _wrapped.get(_context.mutateKey(key));
}

template <typename T>
inline std::string CollectorChannelContextWrap<T>::mutateKeyString(
    const std::string &keystr) {
    try {
        return str(_context.mutateKey(key(keystr)));
    } catch (std::invalid_argument &e) {
        return keystr;
    }
}

} // namespace world