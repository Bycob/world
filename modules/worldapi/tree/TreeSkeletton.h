#pragma once

#include <worldapi/worldapidef.h>

#include "../WeightedSkeletton.h"

namespace world {

/** Contient les information sur la branche d�but�e par ce noeud. */
	class WORLDAPI_EXPORT TreeInfo {
	public :
		TreeInfo(Node<TreeInfo> *myNode);

		int _level = 0;
		/// Taille de la branche termin�e par ce noeud.
		double _size = 0;
		/// Angle par rapport � l'axe Ox (sens direct), en radians
		double _theta = 0;
		/// Angle par rapport � l'axe Oz (des z positifs aux z
		/// n�gatifs), en radians
		double _phi = 0;

		double getWeight() const {
			return this->_myNode->getWeight();
		}

		const Node<TreeInfo> &getNode() const {
			return *_myNode;
		}

	private :
		Node<TreeInfo> *const _myNode;
	};

	class WORLDAPI_EXPORT TreeSkeletton : public WeightedSkeletton<TreeInfo> {
	public:
		TreeSkeletton();

		~TreeSkeletton();

	private :
		friend class TreeSkelettonGenerator;

		friend class TreeGenerator;
	};
}
