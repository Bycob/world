#pragma once

#include <worldapi/worldapidef.h>

#include <math.h>

#include "../Parameters.h"

namespace relief {
	typedef Parameter<std::pair<double, double>, double> diff_param;

	class CustomWorldDifferential : public diff_param {
	public:
		CustomWorldDifferential(double range = 4) : _range(range) {}

		virtual double operator()(const std::pair<double, double> & in) {
			double elevation = in.first;
			double rand = in.second;

			double start = _range * (elevation - 1);
			double startY = tanh(start);
			double endY = tanh(start + _range);

			return (tanh(rand * _range + start) - startY) / (endY - startY) ;
		}

		virtual CustomWorldDifferential * clone() const {
			return new CustomWorldDifferential(*this);
		}
	private:
		double _range;
	};
}