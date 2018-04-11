#include "Material.h"

namespace world {

	Material::Material(const std::string &name) : _name(name) {
		setKd(1, 1, 1);
		setKa(0.2, 0.2, 0.2);
		setKs(1, 1, 1);
	}

	Material::~Material() {

	}

	void Material::setName(const std::string &name) {
		_name = name;
	}

	void Material::setKd(double r, double g, double b) {
		_Kd.set(r, g, b);
	}

	void Material::setKa(double r, double g, double b) {
		_Ka.set(r, g, b);
	}

	void Material::setKs(double r, double g, double b) {
		_Ks.set(r, g, b);
	}

	void Material::setMapKd(const std::string &texName) {
		_mapKd = texName;
	}
}
