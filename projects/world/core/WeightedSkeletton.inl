#include "WeightedSkeletton.h"

#include <iostream>
#include <list>

#include "world/assets/Mesh.h"

namespace world {

template <class T>
SkelettonNode<T>::SkelettonNode(const T &info)
        : _info(info), _parent(nullptr) {}

template <class T> void SkelettonNode<T>::addChild(SkelettonNode<T> *child) {
    _children_or_neighbour.push_back(child);
    child->_parent = this;
}

template <class T>
SkelettonNode<T> *SkelettonNode<T>::createChild(const T &info) {
    SkelettonNode<T> *node = new SkelettonNode<T>(info);
    addChild(node);
    return node;
}

template <class T> void SkelettonNode<T>::remove() {
    // TODO unit tests
    for (auto *child : getChildrenOrNeighboursList()) {
        delete child;
    }

    auto &parentList = _parent->_children_or_neighbour;
    for (auto it{parentList.begin()}; it != parentList.end(); ++it) {
        if (*it == this) {
            parentList.erase(it);
            it--;
        }
    }

    delete this;
}

template <class T>
void SkelettonNode<T>::addNeighbour(SkelettonNode<T> *neighbour) {
    _children_or_neighbour.push_back(neighbour);
    neighbour->_children_or_neighbour.push_back(this);
}

template <class T>
SkelettonNode<T> *SkelettonNode<T>::createNeighbour(const T &info) {
    SkelettonNode<T> *node = new SkelettonNode<T>(info);
    addNeighbour(node);
    return node;
}

template <class T> SkelettonNode<T> *SkelettonNode<T>::getParent() const {
    return _parent;
}

template <class T>
std::vector<SkelettonNode<T> *> SkelettonNode<T>::getChildrenOrNeighboursList()
    const {
    std::vector<SkelettonNode<T> *> result;

    for (auto value : _children_or_neighbour) {
        result.push_back(value);
    }

    return result;
}

template <class T>
WeightedSkeletton<T>::WeightedSkeletton()
        : _primaryNode(new SkelettonNode<T>(T())) {}

template <class T> WeightedSkeletton<T>::~WeightedSkeletton() {
    auto nodeList = getNodeList();

    for (auto *node : nodeList) {
        if (node != _primaryNode.get()) {
            delete node;
        }
    }
}

template <class T>
std::vector<SkelettonNode<T> *> WeightedSkeletton<T>::getNodeList() {
    std::vector<SkelettonNode<T> *> result;

    SkelettonNode<T> *workingNode = _primaryNode.get();
    populateVector(result, workingNode);

    for (SkelettonNode<T> *node : result) {
        node->_mark = false;
    }

    return result;
}

template <typename T> bool WeightedSkeletton<T>::empty() const {
    return _primaryNode->_children_or_neighbour.empty();
}

template <class T> Mesh *WeightedSkeletton<T>::convertToMesh() {
    Mesh *mesh = new Mesh();

    SkelettonNode<T> *workingNode = _primaryNode.get();
    populateMesh(mesh, workingNode);
    resetNode(workingNode);

    return mesh;
}

template <class T>
void WeightedSkeletton<T>::resetNode(SkelettonNode<T> *node) {
    node->_mark = false;
    node->_id = 0;

    for (auto child : node->_children_or_neighbour) {
        if (child->_mark)
            resetNode(child);
    }
}

template <class T>
void WeightedSkeletton<T>::populateMesh(Mesh *mesh, SkelettonNode<T> *node) {

    // Noeud marqué == toutes les connexions ont été établies pour ce noeud.
    if (!node->_mark) {
        node->_mark = true;
        node->_id = mesh->getVerticesCount();

        // Ajout du vertex correspondant
        Vertex vert;
        vert.setPosition(node->getInfo()._position);
        mesh->addVertex(vert);
    }

    // Liste des noeuds à ajouter
    std::list<SkelettonNode<T> *> processList;

    for (SkelettonNode<T> *child : node->_children_or_neighbour) {
        if (!child->_mark) {
            processList.push_back(child);
        }
    }

    // On ajoute les liaisons, puis on appelle récursivement chaque noeud.
    for (SkelettonNode<T> *child : processList) {
        int id = child->_mark ? child->_id : mesh->getVerticesCount();

        Face face;
        face.setID(0, node->_id);
        face.setID(1, id);
        // TODO Faces with only 2 vertices (lines)
        face.setID(2, node->_id);

        mesh->addFace(face);

        populateMesh(mesh, child);
    }
}

template <class T>
void WeightedSkeletton<T>::populateVector(
    std::vector<SkelettonNode<T> *> &vector, SkelettonNode<T> *node) {
    vector.push_back(node);
    node->_mark = true;

    for (SkelettonNode<T> *child : node->_children_or_neighbour) {
        if (!child->_mark)
            populateVector(vector, child);
    }
}

} // namespace world