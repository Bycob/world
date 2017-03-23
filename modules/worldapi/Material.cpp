#include "Material.h"


Material::Material(std::string name) : _name(name) {
	setKd(1, 1, 1);
	setKa(0.2, 0.2, 0.2);
	setKs(1, 1, 1);
}

Material::~Material() {

}

void Material::setKd(double r, double g, double b) {
	_Kd[0] = r;
	_Kd[1] = g;
	_Kd[2] = b;
}

void Material::setKa(double r, double g, double b) {
	_Ka[0] = r;
	_Ka[1] = g;
	_Ka[2] = b;
}

void Material::setKs(double r, double g, double b) {
	_Ks[0] = r;
	_Ks[1] = g;
	_Ks[2] = b;
}

void Material::setMapKd(std::string texName) {
	_mapKd = texName;
}

