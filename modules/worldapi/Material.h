#pragma once

#include <string>
#include <armadillo/armadillo>

#include "worldapidef.h"

class WORLDAPI_EXPORT Material {
public:
	Material(std::string name = "");
	virtual ~Material();

	std::string getName() const {
		return _name;
	}

	void setKd(double r, double g, double b);
	arma::vec3 getKd() {
		return _Kd;
	}
	void setKa(double r, double g, double b);
	arma::vec3 getKa() {
		return _Ka;
	}
	void setKs(double r, double g, double b);
	arma::vec3 getKs() {
		return _Ks;
	}

	void setMapKd(std::string texName);
	std::string getMapKd() {
		return _mapKd;
	}
private :
	std::string _name;

	arma::vec3 _Kd;
	arma::vec3 _Ka;
	arma::vec3 _Ks;
	std::string _mapKd;
};

