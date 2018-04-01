#pragma once

#include "worldapi/worldapidef.h"

#include <string>
#include <vector>
#include <stdexcept>

#include "worldapi/maths/MathsHelper.h"

namespace world {

	class Vertex {
	public :
		Vertex() : _position{}, _normal{}, _texture{} {}

		vec3d getPosition() const { return _position; }

		vec3d getNormal() const { return _normal; }

		vec2d getTexture() const { return _texture; }

		void setPosition(const vec3d &position) {
			_position = position;
		}

		void setPosition(double x, double y, double z) {
			_position = {x, y, z};
		}

		void setNormal(const vec3d &normal) {
			_normal = normal;
		}

		void setNormal(double x, double y, double z) {
			_normal = {x, y, z};
		}

		void setTexture(const vec2d &texture) {
			_texture = texture;
		}

		void setTexture(double x, double y) {
			_texture = {x, y};
		}

	private:
		vec3d _position;
		vec3d _normal;
		vec2d _texture;
	};


	class WORLDAPI_EXPORT Face {

	public :
		Face();

		virtual ~Face();

		void setID(int vert, int id);

		int getID(int vert) const;

		int vertexCount() const;

	private :
		int _ids[3];
	};


	class WORLDAPI_EXPORT Mesh {

	public :
		Mesh();

		virtual ~Mesh();

		void allocateFaces(int capacity);

		Face &getFace(int id);

		const Face &getFace(int id) const;

		const std::vector<Face> &getFaces() const;

		void addFace(const Face &face);

		Face &newFace();

		void allocateVertices(int capacity);

		Vertex &getVertex(int id);

		const Vertex &getVertex(int id) const;

		const std::vector<Vertex> &getVertices() const;

		void addVertex(const Vertex &vert);

		Vertex &newVertex();

		int getCount() const;
	private:
		std::vector<Vertex> _vertices;
		std::vector<Face> _faces;
	};
}
