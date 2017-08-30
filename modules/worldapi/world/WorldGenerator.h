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
	void startGeneration(World & world);
	virtual void generate(World & world) = 0;

	void addNode(WorldGenNode * child);
private:
	void generateChildren(World & world);

	std::vector<WorldGenNode*> _childrens;
};

class PrivateWorldGenerator;

class WORLDAPI_EXPORT WorldGenerator {
public:
    /** Cr�e un g�n�rateur fonctionnel permettant de cr�er un monde
     * relativement complet sans configuration suppl�mentaire.
     * Le g�n�rateur est lui-m�me g�n�r� al�atoirement (Cette fonctionnalit�
     * n'est pas encore impl�ment�e)*/
    static WorldGenerator * createDefaultGenerator();

    WorldGenerator();
    virtual ~WorldGenerator();

	/** G�n�re un monde en utilisant les param�tres pass�s pr�alablement */
    virtual World * generate();

	void addPrimaryNode(WorldGenNode * node);

protected :
	void processGeneration(World & world);

private:
	PrivateWorldGenerator * _internal;

	std::vector<std::unique_ptr<WorldGenNode>> & _nodes();

    friend class WorldGenNode;
};

#endif //WORLD_WORLDGENERATOR_H
