#pragma once

#include "WorldConfig.h"

#include <vector>
#include <memory>
#include <armadillo/armadillo>

#include "math/Vector.h"
#include "math/MathsHelper.h"

#define DEFAULT_WEIGHT 1

namespace world {

class Mesh;

class NodeInfo {
public:
    vec3d _position;

    double _weight;
};

template <class T> class Node {
public:
    ~Node();

    const T &getInfo() const { return _info; }

    T &getInfo() { return _info; }

    void addChild(Node<T> *child);

    Node<T> *createChild(const T &info);

    void addNeighbour(Node<T> *neighbour);

    Node<T> *createNeighbour(const T &info);

    Node<T> *getParent() const;

    std::vector<Node<T> *> getChildrenOrNeighboursList() const;

    const std::vector<Node<T> *> getChildrenOrNeighboursAccess() const {
        return _children_or_neighbour;
    }

    int getChildrenOrNeighboursCount() const {
        return _children_or_neighbour.size();
    }

private:
    template <class S> friend class WeightedSkeletton;

    Node(const T &info);

    // DONNEES SUR LE NOEUD
    T _info;

    // RELATIONS DU NOEUD

    Node<T> *_parent;
    /**Peut contenir la liste des noeuds enfants, ou la liste des
    * noeuds adjacents, selon le mod�le adopt�. */
    std::vector<Node<T> *> _children_or_neighbour;

    // TRAITEMENT DES GRAPHES

    /// Ce champ est utilis� uniquement lors de la suppression d'un
    /// arbre de noeuds.
    bool _deleting = false;
    /// Ce champ est utilis� dans les traitements sur la totalit� du
    /// squelette pour ne pas repasser deux fois au m�me endroit
    bool _mark = false;
    /// L'id du noeud est utilis� temporairement pour mapper tout le
    /// r�seau.
    int _id;
};

/** Un WeightedSkeletton est la structure d'un objet en 3 dimensions.
Il contient un ensemble de noeuds, qui sont en relation soit de voisinage,
soit de parent� (notamment dans le cas des arbres). Chaque noeud poss�de
une position spatiale (x, y, z), et un poids qui peut d�terminer par exemple
la densit� massique au point repr�sent� par le noeud.<p>
Les WeightedSkeletton supportent les structures de noeuds circulaires. Par
exemple, pour des noeuds num�rot�s de 1 � 3, il est possible de r�aliser la
structure suivante : 1 <-> 2 <-> 3 <-> 1<p>
Les liens multiples (1 est deux fois le voisin de 2) et la mixit�
parentage/voisinage n'ont pas �t� test�s. Il est en outre tr�s fortement
d�conseill� d'ajouter des noeuds � plusieurs r�seaux en m�me temps.*/
template <class T> class WeightedSkeletton {
public:
    WeightedSkeletton();

    virtual ~WeightedSkeletton();

    std::vector<Node<T> *> getNodeList();

    Node<T> *getPrimaryNode() const { return _primaryNode.get(); }

    Mesh *convertToMesh();

private:
    void resetNode(Node<T> *node);

    void populateMesh(Mesh *mesh, Node<T> *node);

    void populateVector(std::vector<Node<T> *> &vector, Node<T> *node);

    std::unique_ptr<Node<T>> _primaryNode;
};
} // namespace world
// TODO tester les liens multiples et la mixit� parentage/voisinage

#include "WeightedSkeletton.inl"
