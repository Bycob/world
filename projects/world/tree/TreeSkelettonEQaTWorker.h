#pragma once

#include "world/core/WorldConfig.h"

#include "TreeSkelettonWorker.h"
#include "Tree.h"
#include "TreeSkelettonParameters.h"
#include "TreeSkeletton.h"

namespace world {

/** Equal distribution of branches around the trunk. */
class WORLDAPI_EXPORT TreeSkelettonEQaTWorker : public TreeSkelettonWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    TreeSkelettonEQaTWorker();

    virtual ~TreeSkelettonEQaTWorker();

    TreeSkelettonEQaTWorker *clone() const override;

    void setSeedLocation(const TreeParamd &param);

    void setRootWeight(const TreeParamd &param);

    void setInclination(const TreeParamd &param);

    void setTheta(const TreeParamd &param);

    void setForkingCount(const TreeParami &param);

    void setSize(const TreeParamd &size);

    void setWeight(const TreeParamd &weight);

    void processInstance(TreeInstance &tree, double resolution) override;

    void randomize() override;

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

private:
    // Génération de nombres alétoires uniforme entre 0 et 1.
    Parameter<double> _rng;

    // Localisation du point de départ de l'arbre
    TreeParamd _seedLocation;
    // Poids de départ de l'arbre
    TreeParamd _rootWeight;

    TreeParamd _phi;
    TreeParamd _theta;
    TreeParamd _size;

    TreeParamd _weight;
    // Nombre de branches à chaque division
    TreeParami _count;


    void forkNode(SkelettonNode<TreeInfo> *node);

    void initBranch(SkelettonNode<TreeInfo> *parent,
                    SkelettonNode<TreeInfo> *node);
};
} // namespace world

// Fonctions spécifiques pour les arbres
