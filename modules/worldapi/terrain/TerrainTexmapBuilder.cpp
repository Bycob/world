#include "TerrainTexmapBuilder.h"

#include <iostream>
#include "../maths/mathshelper.h"
#include "../maths/interpolation.h"


using namespace maths;
using namespace arma;

//CLASS TERRAIN_TEXMAP_BUILDER

TerrainTexmapBuilder::TerrainTexmapBuilder(int altMin, int altMax) : _altMin(altMin), _altMax(altMax) {

}

TerrainTexmapBuilder::~TerrainTexmapBuilder() {

}

void TerrainTexmapBuilder::addSlice(int height, std::vector<ColorPart> colorParts) {
	if (height < _altMin || height > _altMax)
		throw std::range_error("Cette slice n'est pas prise en charge par cette textureMap");
	if (colorParts.size() == 0)
		throw std::runtime_error("Une slice doit contenir une ou plusieurs couleurs");

	this->_slices[height] = colorParts;
}

Cube<double> TerrainTexmapBuilder::convertToMap() const {
	if (_slices.begin() == _slices.end()) 
		throw std::runtime_error("Ajouter des couleurs avant de convertir en carte");

	//result[altitude, point (aléatoire), r|g|b]
	Cube<double> result((uword) RESOLUTION, (uword) RESOLUTION, (uword) 3);

	auto sliceIt = _slices.begin();

	auto lowerSlice = sliceIt;
	auto higherSlice = sliceIt;

	for (int h = 0; h < result.n_rows; h++) {
		int altitude = (int) (h * (_altMax - _altMin) / (double)result.n_rows + _altMin);
		
		//Incrémentation des bornes de coupe
		if (higherSlice->first <= altitude && sliceIt != _slices.end()) {
			sliceIt++;
			lowerSlice = higherSlice;

			if (sliceIt != _slices.end()) {
				higherSlice = sliceIt;
			}
		}

		//Déclarations des quatres bornes de coins
		auto lowerfirstPoint = lowerSlice->second.begin();
		auto lowersecondPoint = lowerSlice->second.begin();
		auto higherfirstPoint = higherSlice->second.begin();
		auto highersecondPoint = higherSlice->second.begin();

		double lowerRatioSum = 0;
		double higherRatioSum = 0;

		for (int p = 0; p < result.n_cols; p++) {
			double x = p / (double)result.n_cols;

			//Incrémentation
			if (x >= lowerRatioSum + lowersecondPoint->ratio / 2.0) {
				lowerfirstPoint = lowersecondPoint;
				if (lowersecondPoint != lowerSlice->second.end() - 1) {
					lowersecondPoint++;
					lowerRatioSum += lowerfirstPoint->ratio;
				}
			}
			if (x >= higherRatioSum + highersecondPoint->ratio / 2.0) {
				higherfirstPoint = highersecondPoint;
				if (highersecondPoint != higherSlice->second.end() - 1) {
					highersecondPoint++;
					higherRatioSum += higherfirstPoint->ratio;
				}
			}

			//Interpolation entre les bornes
			//TODO fonctions pour simplifier ça
			result((uword)h, (uword)p, (uword)0) = interpolate(
				lowerSlice->first, interpolate(lowerRatioSum - lowerfirstPoint->ratio / 2.0, lowerfirstPoint->r, lowerRatioSum + lowersecondPoint->ratio / 2.0, lowersecondPoint->r, x),
				higherSlice->first, interpolate(higherRatioSum - higherfirstPoint->ratio / 2.0, higherfirstPoint->r, higherRatioSum + highersecondPoint->ratio / 2.0, highersecondPoint->r, x), altitude);
			result((uword)h, (uword)p, (uword)1) = interpolate(
				lowerSlice->first, interpolate(lowerRatioSum - lowerfirstPoint->ratio / 2.0, lowerfirstPoint->g, lowerRatioSum + lowersecondPoint->ratio / 2.0, lowersecondPoint->g, x),
				higherSlice->first, interpolate(higherRatioSum - higherfirstPoint->ratio / 2.0, higherfirstPoint->g, higherRatioSum + highersecondPoint->ratio / 2.0, highersecondPoint->g, x), altitude);
			result((uword)h, (uword)p, (uword)2) = interpolate(
				lowerSlice->first, interpolate(lowerRatioSum - lowerfirstPoint->ratio / 2.0, lowerfirstPoint->b, lowerRatioSum + lowersecondPoint->ratio / 2.0, lowersecondPoint->b, x),
				higherSlice->first, interpolate(higherRatioSum - higherfirstPoint->ratio / 2.0, higherfirstPoint->b, higherRatioSum + highersecondPoint->ratio / 2.0, highersecondPoint->b, x), altitude);
		}
	}

	return result;
}
