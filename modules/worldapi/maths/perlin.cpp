#include "perlin.h"

#include <armadillo/armadillo>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

#include "mathshelper.h"

using namespace arma;
using namespace maths;

//TODO implémentation des modèles
//TODO class PerlinGenerator ?

//PRIVATE génère un nombre aléatoire entre 0 et 1
double generateRandom() {
	return rand() / (double)RAND_MAX;
}

//PRIVATE
Mat<double> inline defaultModel(int n_rows, int n_cols) {
	Mat<double> model(n_rows, n_cols);
	model.fill(-1);
	return model;
}

void generatePerlinOctave(Mat<double> &output, int offset, float frequency) {
	generatePerlinOctave(output, offset, frequency, defaultModel(output.n_rows, output.n_cols));
}

void generatePerlinOctave(Mat<double> & output, int offset, float frequency, const Mat<double> & model) {
	//Détermination de la taille de la matrice
	int size = std::min(output.n_rows, output.n_cols);

	int period = (size - 1) / frequency; //marche le mieux avec des tailles de pattern de la forme 2^n + 1 et fréquence en 2^i
	if (period == 0) period = 1;

	//Génération des valeurs aléatoires
	for (int x = 0; x < size; x += period) {
		for (int y = 0; y < size; y += period) {

			output(x, y) = generateRandom();
		}
	}

	//Interpolation
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {

			if (x % period != 0 || y % period != 0) {
				//Calcul des bornes
				int borneX1, borneX2, borneY1, borneY2;

				int tileX = x / period;
				int tileY = y / period;

				borneX1 = tileX * period;
				borneY1 = tileY * period;
				borneX2 = borneX1 + period;
				borneY2 = borneY1 + period;

				if (borneX2 >= size) borneX2 = borneX1;
				if (borneY2 >= size) borneY2 = borneY1;

				//Interpolation
				double v1 = interpolate(borneX1, output(borneX1, borneY1), borneX2, output(borneX2, borneY1), x);
				double v2 = interpolate(borneX1, output(borneX1, borneY2), borneX2, output(borneX2, borneY2), x);

				output(x, y) = interpolate(borneY1, v1, borneY2, v2, y);
			}
		}
	}
}

void generatePerlinNoise2D(Mat<double>& output, int offset, int octaveCount, float frequency, float persistence) {
	generatePerlinNoise2D(output, offset, octaveCount, frequency, persistence, defaultModel(output.n_rows, output.n_cols));
}

void generatePerlinNoise2D(Mat<double> &output, int offset, int octaveCount, float frequency, float persistence, const Mat<double> & model) {
	//Initialisation du random avec une seed quelconque
	srand(time(NULL));

	//Détermination de la taille de la matrice
	int size = std::min(output.n_rows, output.n_cols);

	output.fill(0);

	double persistenceSum = 0;
	for (int i = 1; i <= octaveCount; i++) {
		persistenceSum += pow(persistence, i);
	}

	Mat<double> octave(size, size);
	for (int i = 1; i <= octaveCount; i++) {
		generatePerlinOctave(octave, offset * (i - 1), frequency * pow(2, i - 1));
		output += octave * pow(persistence, i) / persistenceSum;
	}
}

Mat<double> generatePerlinNoise2D(int size, int offset, int octaves, float frequency, float persistence) {
	Mat<double> result(size, size);
	generatePerlinNoise2D(result, offset, octaves, frequency, persistence);
	return result;
}
