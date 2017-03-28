#pragma once

#include <worldapi/worldapidef.h>

#include "../GenBase.h"
#include "TreeSkelettonParameters.h"
#include "TreeSkeletton.h"


class WORLDAPI_EXPORT TreeSkelettonGenerator : public GenBase<TreeSkeletton> {
public:
	TreeSkelettonGenerator();
	virtual ~TreeSkelettonGenerator();

	template <class F> void setSeedLocation(const F & seedLocFunc) {
		_seedLocation = std::make_unique<F>(seedLocFunc);
	}

	template <class F> void setRootWeight(const F & rootWeightFunc) {
		_rootWeight = std::make_unique<F>(rootWeightFunc);
	}

	void setConstantInclination(double phi);
	template <class F> void setInclination_wrapped(const F & phiFunc) {
		_phi = std::make_unique<treegen::wrapper_d>(phiFunc);
	}
	template <class F> void setInclination(const F & phiFunc) {
		_phi = std::make_unique<F>(phiFunc);
	}

	template <class F> void setRotationOffset(const F & offsetThetaFunc) {
		_offsetTheta = std::make_unique<F>(offsetThetaFunc);
	}

	void setConstantForkingCount(int count);
	template <class F> void setForkingCount(const F & countFunc) {
		_count = std::make_unique<F>(countFunc);
	}

	void setConstantSizeFactor(double sizeFactor);
	template <class F> void setSizeFactor(const F & sizeFactorFunc) {
		_sizeFactor = std::make_unique<F>(sizeFactorFunc);
	}

	void setConstantMaxForkingLevel(int level);
	template <class F> void setMaxForkingLevel(const F & maxLevelFunc) {
		_maxLevel = std::make_unique<F>(maxLevelFunc);
	}

	template <class F> void setWeight(const F & weight) {
		_weight = std::make_unique<F>(weight);
	}

	virtual std::unique_ptr<TreeSkeletton> generate() const;
private :
	void forkNode(Node<TreeInfo> * node) const;

	// G�n�ration de nombres al�toires uniforme entre 0 et 1. 
	std::unique_ptr<treegen::param_d> _rng;

	// Localisation du point de d�part de l'arbre
	std::unique_ptr<treegen::param_d> _seedLocation;
	// Poids de d�part de l'arbre
	std::unique_ptr<treegen::param_d> _rootWeight;

	std::unique_ptr<treegen::tree_param_d> _phi;
	std::unique_ptr<treegen::tree_param_d> _offsetTheta;
	std::unique_ptr<treegen::tree_param_d> _sizeFactor;

	std::unique_ptr<treegen::tree_param_d> _weight;
	// Nombre de branches � chaque division
	std::unique_ptr<treegen::param_i> _count;
	// Nombre maximum de divisions.
	std::unique_ptr<treegen::param_i> _maxLevel;
};

// Fonctions sp�cifiques pour les arbres



