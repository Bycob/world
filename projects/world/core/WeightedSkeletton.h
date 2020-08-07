#pragma once

#include "WorldConfig.h"

#include <vector>
#include <memory>
#include <armadillo/armadillo>

#include "world/math/Vector.h"
#include "world/math/MathsHelper.h"

#define DEFAULT_WEIGHT 1

namespace world {

class Mesh;

class SkelettonNodeInfo {
public:
    vec3d _position;

    double _weight;
};

template <class T> class SkelettonNode {
public:
    const T &getInfo() const { return _info; }

    T &getInfo() { return _info; }

    void addChild(SkelettonNode<T> *child);

    SkelettonNode<T> *createChild(const T &info);

    /** Remove this node and all its children */
    void remove();

    void addNeighbour(SkelettonNode<T> *neighbour);

    SkelettonNode<T> *createNeighbour(const T &info);

    SkelettonNode<T> *getParent() const;

    std::vector<SkelettonNode<T> *> getChildrenOrNeighboursList() const;

    const std::vector<SkelettonNode<T> *> getChildrenOrNeighboursAccess()
        const {
        return _children_or_neighbour;
    }

    int getChildrenOrNeighboursCount() const {
        return _children_or_neighbour.size();
    }

private:
    template <class S> friend class WeightedSkeletton;

    SkelettonNode(const T &info);

    // DONNEES SUR LE NOEUD
    T _info;

    // RELATIONS DU NOEUD

    SkelettonNode<T> *_parent;
    /**Peut contenir la liste des noeuds enfants, ou la liste des
     * noeuds adjacents, selon le mod�le adopt�. */
    std::vector<SkelettonNode<T> *> _children_or_neighbour;

    // TRAITEMENT DES GRAPHES

    /// Ce champ est utilis� dans les traitements sur la totalit� du
    /// squelette pour ne pas repasser deux fois au m�me endroit
    bool _mark = false;
    /// L'id du noeud est utilis� temporairement pour mapper tout le
    /// r�seau.
    int _id = -1;
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

    std::vector<SkelettonNode<T> *> getNodeList();

    bool empty() const;

    SkelettonNode<T> *getPrimaryNode() const { return _primaryNode.get(); }

    Mesh *convertToMesh();

private:
    void resetNode(SkelettonNode<T> *node);

    void populateMesh(Mesh *mesh, SkelettonNode<T> *node);

    void populateVector(std::vector<SkelettonNode<T> *> &vector,
                        SkelettonNode<T> *node);

    std::unique_ptr<SkelettonNode<T>> _primaryNode;
};
} // namespace world
// TODO tester les liens multiples et la mixit� parentage/voisinage

#include "WeightedSkeletton.inl"
