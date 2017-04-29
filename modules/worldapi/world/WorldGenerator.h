//
// Created by louis on 20/04/17.
//

#ifndef WORLD_WORLDGENERATOR_H
#define WORLD_WORLDGENERATOR_H

#include <worldapi/worldapidef.h>

#include "World.h"

class WorldGenerator;

class WORLDAPI_EXPORT WorldGenNode {
public:
    WorldGenNode(WorldGenerator & generator);
    virtual ~WorldGenNode() = default;

    virtual void addRequiredNodes(World & world) const;
protected:
    const WorldGenerator * _parent;

    /** Ajout un noeud unique du type passé en template au World
     * passé en paramètres, seulement si le World ne contient aucune
     * autre occurence du même noeud. */
    template <typename T>
    void requireUnique(World & world) const {
        if (!T::type.unique())
            throw std::runtime_error("Can't require non-unique node type");
        try {
            world.createNode<T>();
        }
        catch (std::exception & e) {}
    }
};

class WORLDAPI_EXPORT WorldGenerator {
public:
    /** Crée un générateur fonctionnel permettant de créer un monde
     * relativement complet sans configuration supplémentaire.
     * Le générateur est lui-même généré aléatoirement (Cette fonctionnalité
     * n'est pas encore implémentée)*/
    static WorldGenerator * defaultGenerator();

    WorldGenerator();
    WorldGenerator(const WorldGenerator & other);

    ~WorldGenerator();

    World * generate();

private:

    std::vector<std::unique_ptr<WorldGenNode>> _nodes;

    void init(World & world);

    friend class World;
    friend class WorldGenModule;
};

#endif //WORLD_WORLDGENERATOR_H
