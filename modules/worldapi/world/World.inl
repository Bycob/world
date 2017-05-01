
template <typename T> T & World::getUniqueNode()  {
    if (!T::type().unique()) throw std::runtime_error("getUniqueNode() : non-unique type");

    for (auto & node : _nodes()) {
        if (node->type() == T::type()) {
            return *static_cast<T*>(node.get());
        }
    }

    throw std::runtime_error("Node type not found");
}

template <typename T> const T & World::getUniqueNode() const {
    return const_cast<const T &>(const_cast<World *>(this)->getUniqueNode<T>());
}

template <typename T> T & World::createNode()  {
    if (checkNodeTypeInternal(T::type())) {
        T * newNode = new T(this);
        _nodes().emplace_back(newNode);
        return *newNode;
    }
    else {
        throw std::runtime_error("The node can not be created");
    }
}