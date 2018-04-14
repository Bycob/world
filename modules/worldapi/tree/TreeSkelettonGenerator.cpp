#include "TreeSkelettonGenerator.h"

#include <list>

#include "math/MathsHelper.h"

namespace world {

	TreeSkelettonGenerator::TreeSkelettonGenerator()
			: _rng(Params<double>::uniform_real(0, 1)),

			  _seedLocation(TreeParamsd::constant(0)),
			  _rootWeight(TreeParamsd::constant(1)),

			  _phi(TreeParamsd::constant(M_PI / 6.0)),
			  _sizeFactor(TreeParamsd::constant(3.0 / 5.0)),
			  _offsetTheta(TreeParamsd::constant(0)),

			  _weight(TreeParamsd::DefaultWeight()),
			  _count(TreeParamsi::constant(4)),
			  _maxLevel(TreeParamsi::constant(4)) {

	}

	TreeSkelettonGenerator::~TreeSkelettonGenerator() {

	}

	TreeSkelettonGenerator *TreeSkelettonGenerator::clone() const {
		return new TreeSkelettonGenerator(*this);
	}

	void TreeSkelettonGenerator::setSeedLocation(const TreeParamd &param) {
		_seedLocation = param;
	}

	void TreeSkelettonGenerator::setRootWeight(const TreeParamd &param) {
		_rootWeight = param;
	}

	void TreeSkelettonGenerator::setInclination(const TreeParamd &param) {
		_phi = param;
	}

	void TreeSkelettonGenerator::setRotationOffset(const TreeParamd &param) {
		_offsetTheta = param;
	}

	void TreeSkelettonGenerator::setForkingCount(const TreeParami &param) {
		_count = param;
	}

	void TreeSkelettonGenerator::setSizeFactor(const TreeParamd &sizeFactor) {
		_sizeFactor = sizeFactor;
	}

	void TreeSkelettonGenerator::setMaxForkingLevel(const TreeParami &param) {
		_maxLevel = param;
	}

	void TreeSkelettonGenerator::setWeight(const TreeParamd &weight) {
		_weight = weight;
	}

	void TreeSkelettonGenerator::process(Tree &tree) {
		Node<TreeInfo> *primaryNode = tree.getSkeletton().getPrimaryNode();
		primaryNode->setPosition(0, 0, _seedLocation(primaryNode->getInfo(), 0, 0, 0, 0, 0));
		primaryNode->setWeight(_rootWeight(primaryNode->getInfo(), 0, 0, 0, 0, 0));
		primaryNode->getInfo()._level = 0;
		primaryNode->getInfo()._weight = primaryNode->getWeight();

		forkNode(primaryNode);
	}

	void TreeSkelettonGenerator::forkNode(Node<TreeInfo> *node) {
		TreeInfo &info = node->getInfo();
		vec3d pos = node->getPosition();
		std::list<Node<TreeInfo> *> addedNodes;

		//std::cout << info._phi << " " << info._theta << " " << info._level << " " << info._size << std::endl;

		if (info._level != 0) {

			// Détermination du nombre de nouvelles branches
			const int count = _count(info, 0, 0, 0, 0, 0);

			for (int i = 0; i < count; i++) {
				// Détermination de l'angle de rotation
				const double newTheta = (2.0 * M_PI * i) / count + _offsetTheta(info, count, 0, 0, 0, 0);

				// Détermination de la nouvelle inclinaison
				const double phi = _phi(info, count, newTheta, 0, 0, 0);
				const double newPhi = phi + cos(info._theta - newTheta) * info._phi;

				// Détermination du poids de la nouvelle branche
				const double weight = _weight(info, count, newTheta, phi, 0, 0);

				// Détermination de la taille de la nouvelle branche
				const double newSize = info._size * _sizeFactor(info, count, newTheta, phi, weight, 0);

				// Ajout du noeud
				Node<TreeInfo> *newNode = node->createChild(weight,
															pos.x + cos(newTheta) * sin(newPhi) * newSize,
															pos.y + sin(newTheta) * sin(newPhi) * newSize,
															pos.z + cos(newPhi) * newSize);
				addedNodes.push_back(newNode);
				TreeInfo &newInfo = newNode->getInfo();

				newInfo._phi = newPhi;
				newInfo._theta = newTheta;
				newInfo._weight = weight;
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
			const int maxLevel = _maxLevel(info, 0, 0, 0, 0, 0);

			if (info._level < maxLevel) {
				forkNode(addedNode);
			}
		}
	}
}
