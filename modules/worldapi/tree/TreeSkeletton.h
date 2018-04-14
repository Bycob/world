#pragma once

#include "core/WorldConfig.h"

#include "core/WeightedSkeletton.h"

namespace world {

	class WORLDAPI_EXPORT TreeInfo {
	public :
		TreeInfo() {}

		int _level = 0;

		/// Size of the branch terminated by this node.
		double _size = 0;
		/// Rotation around z axis in radians, from 0 to 2 * PI.
		/// 0 means that the branch points toward positive x.
		double _theta = 0;
		/// angle with z axis in radians, from 0 to PI.
		/// 0 means that the branch points toward positive z.
		double _phi = 0;

		/// weight of the branch, ie. thickness.
		double _weight = 1;
	};

	class WORLDAPI_EXPORT TreeSkeletton : public WeightedSkeletton<TreeInfo> {
	public:
		TreeSkeletton();

		~TreeSkeletton() override;
	};
}
