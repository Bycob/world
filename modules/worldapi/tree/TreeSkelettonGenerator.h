#pragma once

#include "core/WorldConfig.h"

#include "ITreeWorker.h"
#include "Tree.h"
#include "TreeSkelettonParameters.h"
#include "TreeSkeletton.h"

namespace world {

	class WORLDAPI_EXPORT TreeSkelettonGenerator : public ITreeWorker {
	public:
		TreeSkelettonGenerator();

		virtual ~TreeSkelettonGenerator();

		TreeSkelettonGenerator *clone() const override;

		void setSeedLocation(const TreeParamd &param);

		void setRootWeight(const TreeParamd &param);

		void setInclination(const TreeParamd &param);

		void setRotationOffset(const TreeParamd &param);

		void setForkingCount(const TreeParami &param);

		void setSizeFactor(const TreeParamd &sizeFactor);

		void setMaxForkingLevel(const TreeParami &param);

		void setWeight(const TreeParamd &weight);

		void process(Tree &tree) override;

	private :
		void forkNode(Node<TreeInfo> *node);

		// G�n�ration de nombres al�toires uniforme entre 0 et 1.
		Parameter<double> _rng;

		// Localisation du point de d�part de l'arbre
		TreeParamd _seedLocation;
		// Poids de d�part de l'arbre
		TreeParamd _rootWeight;

		TreeParamd _phi;
		TreeParamd _offsetTheta;
		TreeParamd _sizeFactor;

		TreeParamd _weight;
		// Nombre de branches � chaque division
		TreeParami _count;
		// Nombre maximum de divisions.
		TreeParami _maxLevel;
	};
}

// Fonctions sp�cifiques pour les arbres



