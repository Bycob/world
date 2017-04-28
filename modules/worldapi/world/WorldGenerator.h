//
// Created by louis on 20/04/17.
//

#ifndef WORLD_WORLDGENERATOR_H
#define WORLD_WORLDGENERATOR_H

#include <worldapi/worldapidef.h>

#include "World.h"

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

    World * createWorld();

private:

    void init(World & world);

    friend class World;
};

#endif //WORLD_WORLDGENERATOR_H
