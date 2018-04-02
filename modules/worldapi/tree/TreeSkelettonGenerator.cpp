#define _USE_MATH_DEFINES

#include <list>
#include <math.h>

#include "math/MathsHelper.h"

#include "TreeSkelettonGenerator.h"


namespace world {
	using namespace tree;

	TreeSkelettonGenerator::TreeSkelettonGenerator()
			: _rng(std::make_unique<uniform_d>(0, 1)),

			  _seedLocation(std::make_unique<const_d>(0)),
			  _rootWeight(std::make_unique<const_d>(1)),

			  _phi(std::make_unique<wrapper_d>(const_d(M_PI / 6.0))),
			  _sizeFactor(std::make_unique<wrapper_d>(const_d(3.0 / 5.0))),
			  _offsetTheta(std::make_unique<wrapper_d>(const_d(0))),

			  _weight(std::make_unique<DefaultWeightParameter>()),
			  _count(std::make_unique<const_i>(4)),
			  _maxLevel(std::make_unique<const_i>(4)) {

	}

	TreeSkelettonGenerator::TreeSkelettonGenerator(const TreeSkelettonGenerator &other)
			: _rng(other._rng->clone()),

			  _seedLocation(other._seedLocation->clone()),
			  _rootWeight(other._rootWeight->clone()),

			  _phi(other._phi->clone()),
			  _sizeFactor(other._sizeFactor->clone()),
			  _offsetTheta(other._offsetTheta->clone()),

			  _weight(other._weight->clone()),
			  _count(other._count->clone()),
			  _maxLevel(other._maxLevel->clone()) {

	}

	TreeSkelettonGenerator::~TreeSkelettonGenerator() {

	}

	TreeSkelettonGenerator *TreeSkelettonGenerator::clone() const {
		return new TreeSkelettonGenerator(*this);
	}

	void TreeSkelettonGenerator::setConstantInclination(double phi) {
		_phi = std::make_unique<wrapper_d>(const_d(phi));
	}

	void TreeSkelettonGenerator::setConstantForkingCount(int count) {
		_count = std::make_unique<const_i>(count);
	}

	void TreeSkelettonGenerator::setConstantSizeFactor(double sizeFactor) {
		_sizeFactor = std::make_unique<wrapper_d>(const_d(sizeFactor));
	}

	void TreeSkelettonGenerator::setConstantMaxForkingLevel(int level) {
		_maxLevel = std::make_unique<const_i>(level);
	}

	TreeSkeletton *TreeSkelettonGenerator::generate() {
		TreeSkeletton *result = new TreeSkeletton();

		Node<TreeInfo> *primaryNode = result->getPrimaryNode();
		primaryNode->setPosition(0, 0, (*_seedLocation)(primaryNode->getInfo()));
		primaryNode->setWeight((*_rootWeight)(primaryNode->getInfo()));
		primaryNode->getInfo()._level = 0;

		forkNode(primaryNode);

		return result;
	}

	void TreeSkelettonGenerator::forkNode(Node<TreeInfo> *node) {
		TreeInfo &info = node->getInfo();
		vec3d pos = node->getPosition();
		std::list<Node<TreeInfo> *> addedNodes;

		//std::cout << info._phi << " " << info._theta << " " << info._level << " " << info._size << std::endl;

		if (info._level != 0) {

			// Détermination du nombre de nouvelles branches
			const int count = (*_count)(info);

			for (int i = 0; i < count; i++) {
				// Détermination de l'angle de rotation
				const double newTheta = (2.0 * M_PI * i) / count + (*_offsetTheta)(info, count);

				// Détermination de la nouvelle inclinaison
				const double phi = (*_phi)(info, count, newTheta);
				const double newPhi = phi + cos(info._theta - newTheta) * info._phi;

				// Détermination du poids de la nouvelle branche
				const double weight = (*_weight)(info, count, newTheta, phi);

				// Détermination de la taille de la nouvelle branche
				const double newSize = info._size * (*_sizeFactor)(info, count, newTheta, phi, weight);

				// Ajout du noeud
				Node<TreeInfo> *newNode = node->createChild(weight,
															pos.x + cos(newTheta) * sin(newPhi) * newSize,
															pos.y + sin(newTheta) * sin(newPhi) * newSize,
															pos.z + cos(newPhi) * newSize);
				addedNodes.push_back(newNode);
				TreeInfo &newInfo = newNode->getInfo();

				newInfo._phi = newPhi;
				newInfo._theta = newTheta;
			}
		} else {
			// On fait le tronc. Juste le tronc.
			addedNodes.push_back(node->createChild(1.0, 0, 0, 1));
		}

		for (auto addedNode : addedNodes) {
			// Mise à jour des variables
			TreeInfo &newInfo = addedNode->getInfo();
			newInfo._level = info._level + 1;
			newInfo._size = vec3d::length(node->getPosition(), addedNode->getPosition());

			// On fork ensuite tous les nodes créés.
			const int maxLevel = (*_maxLevel)(info);

			if (info._level < maxLevel) {
				forkNode(addedNode);
			}
		}
	}
}
