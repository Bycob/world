#pragma once

#include "core/WorldConfig.h"

#include "core/Parameters.h"

namespace world {

	typedef Parameter<double, std::pair<double, double>> diff_law;

	inline diff_law CustomWorldDifferential(double range = 4) {
		diff_law ret;
		ret.setFunction([range](const std::pair<double, double> &in) {
			double elevation = in.first;
			double rand = in.second;

			double start = range * (elevation - 1);
			double startY = tanh(start);
			double endY = tanh(start + range);

			return (tanh(rand * range + start) - startY) / (endY - startY);
		});
		return ret;
	}
}
