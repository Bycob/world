//
// Created by louis on 20/04/17.
//

#ifndef WORLD_WORLDGENERATOR_H
#define WORLD_WORLDGENERATOR_H

#include <worldapi/worldapidef.h>

#include "World.h"

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

    World * createWorld();

private:

    void init(World & world);

    friend class World;
};

#endif //WORLD_WORLDGENERATOR_H
