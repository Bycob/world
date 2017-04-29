
template <typename T> T & World::getUniqueNode()  {
    if (!T::type.isUnique()) throw std::runtime_error("getUniqueNode() : non-unique type");

    for (auto & node : _nodes) {
        if (node->type() == T::type) {
            return *static_cast<T*>(node.get());
        }
    }

    throw std::runtime_error("Node type not found");
}

template <typename T> T & World::createNode()  {
    if (checkNodeTypeInternal(T::type)) {
        T * newNode = new T(this);
        _nodes.emplace_back(newNode);
        return *newNode;
    }
    else {
        throw std::runtime_error("The node can not be created");
    }
}