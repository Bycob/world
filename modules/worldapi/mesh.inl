
// FACE

template<> inline std::vector<int> & Face::getIDsInternal<VType::POSITION>() {
    return _positionIDs;
}

template<> inline std::vector<int> & Face::getIDsInternal<VType::NORMAL>() {
    return _normalIDs;
}

template<> inline std::vector<int> & Face::getIDsInternal<VType::TEXTURE>() {
    return _textureIDs;
}

template<> inline std::vector<int> & Face::getIDsInternal<VType::PARAM>() {
    return _paramIDs;
}

template <VType T> inline int Face::getID(int vert) const {
    if (vert >= _vcount) throw std::runtime_error("Erreur : sommet inexistant");
    return getIDs<T>()[vert];
}

template <VType T> inline void Face::setID(int vert, int id) {
    if (vert >= _vcount) throw std::runtime_error("Erreur : sommet inexistant");
    getIDsInternal<T>()[vert] = id;
}

template <VType T> inline const std::vector<int> & Face::getIDs() const {
    return const_cast<Face *>(this)->getIDsInternal<T>();
}



// MESH

template<> inline std::vector<Vertex<VType::POSITION>> & Mesh::getList<VType::POSITION>() {
    return _positions;
}

template<> inline std::vector<Vertex<VType::NORMAL>> & Mesh::getList<VType::NORMAL>() {
    return _normals;
}

template<> inline std::vector<Vertex<VType::TEXTURE>> & Mesh::getList<VType::TEXTURE>() {
    return _textures;
}

template<> inline std::vector<Vertex<VType::PARAM>> & Mesh::getList<VType::PARAM>() {
    return _params;
}

template<VType T> inline const std::vector<Vertex<T>> & Mesh::getVertices() const {
    return const_cast<Mesh*>(this)->getList<T>();
}

template <VType T> inline int Mesh::getCount() const {
    return const_cast<Mesh *>(this)->getList<T>().size();
}

template <VType T> inline void Mesh::clearVertices() {
    getList<T>().clear();
}

template <VType T> inline void Mesh::allocateVertices(int count) {
    getList<T>().reserve(count);
}

template <VType T> inline void Mesh::addVertex(Vertex<T> & vertex) {
    std::vector<Vertex<T>> & vertexList = getList<T>();
    int id = vertexList.size();
    vertexList.push_back(vertex);
    vertexList[id].setID(id);
}
