#pragma once

#include <worldapi/worldapidef.h>

#include <map>
#include <vector>
#include <armadillo/armadillo>

namespace world {

#define RESOLUTION 256

	struct WORLDAPI_EXPORT ColorPart {

		double r, g, b;
		double ratio;

		ColorPart(double r, double g, double b) : ColorPart(r, g, b, 1) {}

		ColorPart(double r, double g, double b, double ratio) : r(r), g(g), b(b), ratio(ratio) {}

		ColorPart(const ColorPart &color, double ratio) : r(color.r), g(color.g), b(color.b), ratio(ratio) {}

		bool operator==(const ColorPart &other) { return r == other.r && g == other.g && b == other.b; }
	};

/* Contient les informations relatives à la texture du terrain en fonction
de l'altitude. Travaille à l'échelle macro (précision de l'ordre du mètre)*/
	class WORLDAPI_EXPORT TerrainTexmapBuilder {

	public:
		TerrainTexmapBuilder(int altMin, int altMax);

		virtual ~TerrainTexmapBuilder();

		void addSlice(int height, std::vector<ColorPart> colorParts);

		arma::Cube<double> convertToMap() const;

	private :
		//Les altitudes minimum et maximum prise en charge par le générateur.
		int _altMin, _altMax;
		std::map<int, std::vector<ColorPart>> _slices;
	};
}
