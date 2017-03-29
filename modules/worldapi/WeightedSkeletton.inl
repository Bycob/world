#include <iostream>
#include <list>

#include "mesh.h"
#include "WeightedSkeletton.h"

template <class T> Node<T>::Node(double weight, double x, double y, double z) : 
	_weight(weight), _x(x), _y(y), _z(z), _parent(nullptr), _info(std::make_unique<T>(this)) {
	
}

template <class T> Node<T>::~Node() {
	//Passage en suppression
	_deleting = true;

	for (int i = 0; i < _children_or_neighbour.size(); i++) {
		Node<T> * node = _children_or_neighbour.at(i);

		if (node != nullptr) {
			//On coupe d'abord les liens avec le noeud qui va être supprimé
			if (node->_parent != this) {
				auto &vec = node->_children_or_neighbour;

				for (auto it = vec.begin(); it != vec.end(); it++) {
					if (*it == this) {
						*it = nullptr;
					}
				}
			}

			//puis on le supprime
			if (!node->_deleting) {
				delete node;
				_children_or_neighbour[i] = nullptr;
			}
		}
	}
}

template<class T> void Node<T>::setWeight(double weight) {
	_weight = weight;
}

template<class T> void Node<T>::setPosition(double x, double y, double z) {
	_x = x;
	_y = y;
	_z = z;
}

template<class T> maths::vec3d Node<T>::getPosition() const {
	return maths::vec3d(_x, _y, _z);
}

template<class T> void Node<T>::addChild(Node<T> * child) {
	_children_or_neighbour.push_back(child);
	child->_parent = this;
}

template<class T> Node<T> * Node<T>::createChild(double weight, double x, double y, double z) {
	Node<T> * node = new Node<T>(weight, x, y, z);
	addChild(node);
	return node;
}

template<class T> void Node<T>::addNeighbour(Node<T> * neighbour) {
	_children_or_neighbour.push_back(neighbour);
	neighbour->_children_or_neighbour.push_back(this);
}

template<class T> Node<T> * Node<T>::createNeighbour(double weight, double x, double y, double z) {
	Node<T> * node = new Node<T>(weight, x, y, z);
	addNeighbour(node);
	return node;
}

template <class T> Node<T> * Node<T>::getParent() const {
	return _parent;
}

template<class T> std::vector<Node<T>*> Node<T>::getChildrenOrNeighboursList() const {
	std::vector<Node<T>*> result;

	for (auto value : _children_or_neighbour) {
		result.push_back(value);
	}

	return result;
}






template <class T> WeightedSkeletton<T>::WeightedSkeletton() : 
	_primaryNode(new Node<T>()) {


}

template <class T> WeightedSkeletton<T>::~WeightedSkeletton() {

}

template <class T> std::vector<Node<T> *> WeightedSkeletton<T>::getNodeList() {
	std::vector<Node<T> *> result;

	Node<T> * workingNode = _primaryNode.get();
	populateVector(result, workingNode);

	for (Node<T> * node : result) {
		node->_mark = false;
	}

	return result;
}

template<class T> Mesh * WeightedSkeletton<T>::convertToMesh() {
	Mesh * mesh = new Mesh();

	Node<T> * workingNode = _primaryNode.get();
	populateMesh(mesh, workingNode);
	resetNode(workingNode);

	return mesh;
}

template<class T> void WeightedSkeletton<T>::resetNode(Node<T>* node) {
	node->_mark = false;
	node->_id = 0;
	
	for (auto child : node->_children_or_neighbour) {
		if (child->_mark) resetNode(child);
	}
}

template <class T> void WeightedSkeletton<T>::populateMesh(Mesh * mesh, Node<T> * node) {

	// Noeud marqué == toutes les connexions ont été établies pour ce noeud.
	if (!node->_mark) {
		node->_mark = true;
		node->_id = mesh->getCount<VType::POSITION>();

		//Ajout du vertex correspondant
		Vertex<VType::POSITION> vert;
		vert.add(node->_x).add(node->_y).add(node->_z);
		mesh->addVertex(vert);
	}

	// Liste des noeuds à ajouter
	std::list<Node<T> *> processList;

	for (Node<T> * child : node->_children_or_neighbour) {
		if (!child->_mark) {
			processList.push_back(child);
		}
	}

	// On ajoute les liaisons, puis on appelle récursivement chaque noeud.
	for (Node<T> * child : processList) {
		int id = child->_mark ? child->_id : mesh->getCount<VType::POSITION>();

		Face face;
		face.addVertexUniqueID(node->_id);
		face.addVertexUniqueID(id);
		
		mesh->addFace(face);

		populateMesh(mesh, child);
	}
}

template<class T> void WeightedSkeletton<T>::populateVector(std::vector<Node<T> *> & vector, Node<T>* node) {
	vector.push_back(node);
	node->_mark = true;
	
	for (Node<T> * child : node->_children_or_neighbour) {
		if (!child->_mark) populateVector(vector, child);
	}
}