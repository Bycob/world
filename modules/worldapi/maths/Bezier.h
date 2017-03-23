#pragma once

#include "mathshelper.h"

namespace maths {
	
	class BezierCurve {
	public:
		BezierCurve(vec3d pt1, vec3d pt2, vec3d pt1Weight, vec3d pt2Weight) : 
			_pt1(pt1), _pt2(pt2), _pt1Weight(pt1Weight), _pt2Weight(pt2Weight) {}

		/** Retourne le point situé sur la courbe, de paramètre t.
		Ce paramètre peut représenter "l'avancement" du point sur la
		courbe : si t = 0 alors on retourne le point pt1, si t = 1
		on retourne le point pt2.*/
		vec3d getPointAt(double t) const {
			const vec3d &pt1 = _pt1;
			const vec3d pt2 = _pt1 + _pt1Weight;
			const vec3d pt3 = _pt2 + _pt2Weight;
			const vec3d &pt4 = _pt2;

			double omt = 1 - t;
			
			double b1 = omt * omt * omt;
			double b2 = omt * omt * t;
			double b3 = omt * t * t;
			double b4 = t * t * t;

			return pt1 * b1 + pt2 * (b2 * 3) + pt3 * (b3 * 3) + pt4 * b4;
		}

		vec3d getDerivativeAt(double t) const {
			const vec3d &pt12 = _pt1Weight;
			const vec3d pt23 = (_pt2 + _pt2Weight) - (_pt1 + _pt1Weight);
			const vec3d &pt43 = _pt2Weight;

			double omt = 1 - t;
			
			double b1 = omt * omt;
			double b2 = omt * t;
			double b3 = t * t;

			return pt12 * (3 * b1) + pt23 * (6 * b2) - pt43 * (3 * b3);
		}

	private:
		vec3d _pt1;
		vec3d _pt1Weight;
		vec3d _pt2;
		vec3d _pt2Weight;
	};
}