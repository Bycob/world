//
// Created by louis on 20/04/17.
//

#ifndef WORLD_WORLDGENERATOR_H
#define WORLD_WORLDGENERATOR_H

#include <worldapi/worldapidef.h>

#include "World.h"
#include "IPointOfView.h"
#include "../maths/Vector.h"

class WorldGenerator;

class WORLDAPI_EXPORT WorldGenNode {
public:
    WorldGenNode(WorldGenerator * generator);
    virtual ~WorldGenNode() = default;

	virtual void expand(World & world, const IPointOfView &location) = 0;

    virtual void addRequiredNodes(World & world) const = 0;
    virtual WorldGenNode * clone(WorldGenerator * newParent) = 0;
protected:
    WorldGenerator * _parent;

    /** Ajout un noeud unique du type pass� en template au World
     * pass� en param�tres, seulement si le World ne contient aucune
     * autre occurence du m�me noeud. */
    template <typename T>
    T & requireUnique(World & world) const {
        if (!T::type().unique())
            throw std::runtime_error("Can't require non-unique node type");
        try {
            return world.createNode<T>();
        }
        catch (std::exception & e) {
			return world.getUniqueNode<T>();
		}
    }

    friend class WorldGenerator;
};

class PrivateWorldGenerator;

class WORLDAPI_EXPORT WorldGenerator {
public:
    /** Cr�e un g�n�rateur fonctionnel permettant de cr�er un monde
     * relativement complet sans configuration suppl�mentaire.
     * Le g�n�rateur est lui-m�me g�n�r� al�atoirement (Cette fonctionnalit�
     * n'est pas encore impl�ment�e)*/
    static WorldGenerator * defaultGenerator();

    WorldGenerator();
    WorldGenerator(const WorldGenerator & other);

    ~WorldGenerator();

    World * generate();

	/** Etend le monde � la position indiqu�e, g�n�rant du nouveau contenu
	et/ou des niveaux de d�tails suppl�mentaires. */
	void expand(World & world, const IPointOfView &location);

private:

	PrivateWorldGenerator * _internal;

	std::vector<std::unique_ptr<WorldGenNode>> & _nodes();
    void init(World & world);

    friend class World;
    friend class WorldGenModule;
};

#endif //WORLD_WORLDGENERATOR_H
