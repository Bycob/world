namespace world {

template <typename T>
inline CollectorChannel<T> &Collector::addStorageChannel() {
    return addCustomChannel<T, CollectorChannel<T>>();
}

template <typename T, typename CustomChannel, typename... Args>
inline CustomChannel &Collector::addCustomChannel(Args... args) {
    int type = typeid(ICollectorChannel<T>).hash_code();
#ifdef _MSC_VER
    auto ptr = std::make_shared<CustomChannel>(args...);
#else
    auto ptr = std::make_unique<CustomChannel>(args...);
#endif
    CustomChannel &ref = *ptr;
    _channels.emplace(type, std::move(ptr));
    return ref;
}

template <typename T> inline bool Collector::hasStorageChannel() const {
    auto it = _channels.find(typeid(ICollectorChannel<T>).hash_code());
    return it != _channels.end() &&
           dynamic_cast<CollectorChannel<T> *>(it->second) != nullptr;
}

template <typename T>
inline CollectorChannel<T> &Collector::getStorageChannel() {
    // TODO throw right exception if not present
    auto &channel = *_channels[typeid(ICollectorChannel<T>).hash_code()];
    return dynamic_cast<CollectorChannel<T> &>(channel);
}

// ====== CollectorChannel

template <typename T> inline CollectorChannel<T>::CollectorChannel() = default;

template <typename T>
inline void CollectorChannel<T>::put(const ItemKey &key, const T &item) {
#ifdef _MSC_VER
	_items[key] = std::make_shared<T>(item);
#else
	_items[key] = std::make_unique<T>(item);
#endif
}

template <typename T>
inline bool CollectorChannel<T>::has(const ItemKey &key) const {
	return _items.find(key) != _items.end();
}

template <typename T>
inline void CollectorChannel<T>::remove(const ItemKey &key) {
	_items.erase(key);
}

template <typename T>
inline const T &CollectorChannel<T>::get(const ItemKey &key) const {
	return *_items.at(key);
}

template <typename T>
inline std::string CollectorChannel<T>::keyToString(const ItemKey &key) const {
	return ItemKeys::toString(key);
}

template <typename T> inline void CollectorChannel<T>::reset() {
	_items.clear();
}

template <typename T>
inline CollectorChannelIterator<T> CollectorChannel<T>::begin() {
	return CollectorChannelIterator<T>(_items.begin());
}

template <typename T>
inline CollectorChannelIterator<T> CollectorChannel<T>::end() {
	return CollectorChannelIterator<T>(_items.end());
}

// ====== CollectorChannelIterator

#ifdef _MSC_VER
template <typename T>
inline CollectorChannelIterator<T>::CollectorChannelIterator(
	typename std::map<ItemKey, std::shared_ptr<T>>::iterator it)
	: _it(it) {}
#else
template <typename T>
inline CollectorChannelIterator<T>::CollectorChannelIterator(
	typename std::map<ItemKey, std::unique_ptr<T>>::iterator it)
	: _it(it) {}
#endif

template <typename T>
inline CollectorChannelIterator<T> &CollectorChannelIterator<T>::operator++() {
	_it++;
	return *this;
}

template <typename T>
inline std::pair<ItemKey, T *> CollectorChannelIterator<T>::operator*() const {
	return std::make_pair(_it->first, _it->second.get());
}

template <typename T>
inline bool CollectorChannelIterator<T>::operator==(
	const CollectorChannelIterator<T> &other) const {
	return _it == other._it;
}

template <typename T>
inline bool CollectorChannelIterator<T>::operator!=(
	const CollectorChannelIterator<T> &other) const {
	return _it != other._it;
}

} // namespace world
