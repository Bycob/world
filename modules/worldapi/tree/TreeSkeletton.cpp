#include "TreeSkeletton.h"

namespace world {

	TreeInfo::TreeInfo(Node<TreeInfo> *myNode) : _myNode(myNode) {
		;
	}

	TreeSkeletton::TreeSkeletton() {}

	TreeSkeletton::~TreeSkeletton() = default;
}