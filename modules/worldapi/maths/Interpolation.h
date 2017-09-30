
#ifndef WORLD_INTERPOLATION_H
#define WORLD_INTERPOLATION_H

#include <worldapi/worldapidef.h>

#include <utility>

#include "MathsHelper.h"

namespace maths {
	class WORLDAPI_EXPORT CosineInterpolation : public Function<double> {
	public:
		virtual double operator() (double x) const;
	};

	class WORLDAPI_EXPORT LinearInterpolation : public Function<double> {
	public:
		virtual double operator() (double x) const;
	};

	double WORLDAPI_EXPORT interpolate(double x1, double y1, double x2, double y2, double x, const Function<double> * func);
    double WORLDAPI_EXPORT interpolateCosine(double x1, double y1, double x2, double y2, double x);
    double WORLDAPI_EXPORT interpolateLinear(double x1, double y1, double x2, double y2, double x);



	// https://en.wikipedia.org/wiki/Inverse_distance_weighting
	/** Cette classe permet de réaliser une interpolation en tout point
	de l'espace de départ, à partir des valeurs connues en certains points
	totalement arbitraires.*/
	template <typename T_Pts, typename T_Data>
	class IDWInterpolator {
	public:
		typedef std::pair<T_Pts, T_Data> DataPoint;

		IDWInterpolator(uint32_t p = 1, double radius = 1000000) : _p(p), _radius(radius) {}

		void setP(uint32_t p) {
			_p = p;
		}

		void setData(const std::vector<DataPoint> data) {
			_data = std::vector<DataPoint>(data);
		}

		void addData(T_Pts pt, T_Data data) {
			_data.emplace_back(pt, data);
		}

		T_Data getData(const T_Pts pt) const {
			T_Data sum; // TODO = T_Data::zero();
			double wSum = 0;

			for (const DataPoint & dp : _data) {
				// TODO resoudre les warnings de type
				auto length = T_Pts::length(pt, dp.first);
				
				if (length < std::numeric_limits<decltype(length)>::epsilon()) {
					return dp.second;
				}
				else {
					double weight = length < _radius ? 1 / pow(length, _p) : 0;

					wSum = wSum + weight;
					sum = sum + dp.second * weight;
				}
			}

			if (wSum < std::numeric_limits<double>::epsilon()) {
				// Soit il n'y a pas de points, soit on en est trop loin. Dans les deux cas, on retourne 0.
				return sum;
			}

			return sum * (1 / wSum);
		}
	private:
		uint32_t _p;
		double _radius;
		std::vector<DataPoint> _data;
	};
}

#endif //WORLD_INTERPOLATION_H
