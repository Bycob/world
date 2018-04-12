#include "AltitudeTexturer.h"

namespace world {

	AltitudeTexturer::AltitudeTexturer(int pixelPerVertex)
		: _pixelPerVertex(pixelPerVertex), _colorMap({ 513, 65 }), _rng(time(NULL)){

	}

	ColorMap &AltitudeTexturer::getColorMap() {
		return _colorMap;
	}

	void AltitudeTexturer::process(Terrain & terrain) {
		int res = terrain.getResolution();
		int imgSize = _pixelPerVertex * (res - 1);
		Image texture(imgSize, imgSize, ImageType::RGB);

		std::uniform_real_distribution<double> positive(0, 1);
		std::uniform_real_distribution<double> jitter(-1, 1);

		for (int x = 0; x < imgSize; x++) {
			for (int y = 0; y < imgSize; y++) {
				double xd = (double)x / (imgSize - 1);
				double yd = (double)y / (imgSize - 1);

				// get the parameters to pick in the colormap
				double altitude = clamp(terrain.getExactHeightAt(xd, yd) + jitter(_rng) * 0.01, 0, 1);
				double climate = positive(_rng);

				// pick the color
				Color4d color = _colorMap.getColorAt({ altitude, climate });

				// jitter the color and set in the texture
				double j = 5. / 255.;
				texture.rgb(x, y).setf(
					clamp(color._r + jitter(_rng) * j, 0, 1),
					clamp(color._g + jitter(_rng) * j, 0, 1),
					clamp(color._b + jitter(_rng) * j, 0, 1)
				);
			}
		}

		terrain.setTexture(std::move(texture));
	}

	void AltitudeTexturer::process(Terrain & terrain, ITerrainWorkerContext & context) {
		process(terrain);
	}
}
