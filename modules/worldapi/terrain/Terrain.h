#pragma once

#include "core/WorldConfig.h"

#include <armadillo/armadillo>
#include <vector>
#include <memory>

#include "math/Interpolation.h"
#include "math/MathsHelper.h"
#include "math/BoundingBox.h"
#include "assets/Mesh.h"
#include "assets/Image.h"

namespace world {

	/* NB : Toutes les ressources générées par cette classe utilisent le COLUMN MAJOR ORDER,
	c'est à dire que la première colonne de matrice armadillo correspondra à la première
	ligne d'un fichier image correspondant.
	-> x indique l'indice de la ligne,
	-> y indique l'indice de la colonne.*/

	class WORLDAPI_EXPORT TerrainGenerator;

	/** A Terrain is a squared Heightmap with spatial bounds and
	 * a bunch of convenience methods. A terrain can be converted
	 * to a mesh, or an image, depending on what use one needs.
	 * The terrain can embed a texture. */
	class WORLDAPI_EXPORT Terrain {

	public :
		explicit Terrain(int size);

		explicit Terrain(const arma::Mat<double> &data);

		Terrain(const Terrain &terrain);

		Terrain(Terrain &&terrain);

		virtual ~Terrain();

		Terrain &operator=(const Terrain &terrain);

		void setBounds(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax);

		const BoundingBox &getBoundingBox() const;

		int getSize() const {
			return static_cast<int>(_array.n_rows);
		}

		double &operator()(int x, int y);

		/** Gets the height from the height map case which is the
		 * nearest to (x, y).  */
		double getRawHeight(double x, double y) const;

		/** Gets the height of the terrain at the specified point,
		 * using the given interpolation method. This method perform
		 * an interpolation on both x and y axis between the nearest
		 * heightmap points around (x, y). */
		double getInterpolatedHeight(double x, double y, const Interpolation::interpFunc &func) const;

		/** Gets the height of the terrain at the specified point.
		 * The height given by this method corresponds to the exact
		 * height of the terrain mesh at the point (x, y), given
		 * that it's xy bounds are [0,1] on both axis.
		 * @param x coordinate in terrain-coordinates space, ie. from
		 * 0 to 1.
		 * @param y see above */
		double getExactHeightAt(double x, double y) const;

		// ------ IO

		/** Creates a mesh from this heightmap. Each vertex represents a
		 * case in the heightmap. xy bounds of the terrain are
		 * determined from the bounding box, and the mesh is scaled along
		 * z-axis depending of the z-size of the bounding box. */
		Mesh *createMesh() const;

		/** Creates a mesh from this heightmap. Each vertex represents a
		 * case in the heightmap. The mesh is centred on the origin, and
		 * scaled to the given dimensions. */
		Mesh *createMesh(double sizeX, double sizeY, double sizeZ) const;

		Mesh *
		createMesh(double offsetX, double offsetY, double offsetZ, double sizeX, double sizeY, double sizeZ) const;

		//Méthodes pour la conversion du terrain en image.
		Image createImage() const;

		//Raw map TODO raw data stream ?
		char *getRawData(int &rawDataSize, float height = 1, float offset = 0) const;

		int getRawDataSize() const;

		optional<const Image &> getTexture() const;

	private :

		BoundingBox _bbox;
		arma::Mat<double> _array;
		std::unique_ptr<Image> _texture;

		// ------

		friend class TerrainGenerator;

		friend class PerlinTerrainGenerator;

		friend class TerrainManipulator;

		vec2i getPixelPos(double x, double y) const;
	};
}
