#pragma once

#include <random>
#include <time.h>

#include "worldapidef.h"

long default_seed() {
	return (long) time(NULL);
}

//FONCTIONS-PARAMETRES

template <typename I, typename T>
class Parameter {
public:
	virtual T operator()(const I & in) = 0;
};

template <typename I, typename T>
class ConstantParameter : public Parameter<I, T> {
public :
	ConstantParameter(T value) : _value(value) {}

	virtual T operator()(const I &in) {
		return _value;
	}
private :
	T _value;
};

template <typename I, typename T>
class RandomParameter : public Parameter<I, T> {
public :
	RandomParameter() : _rng(default_seed()) {}
protected :
	std::mt19937 _rng;
};

template <typename I>
class GaussianParameter : public RandomParameter<I, double> {
public :
	GaussianParameter(double median, double deviation) : _random(median, deviation) {}

	virtual double operator()(const I & in) {
		return this->_random(this->_rng);
	}
private :
	std::normal_distribution<double> _random;
};

template <typename I>
class UniformRandomIntegerParameter : public RandomParameter<I, int> {
public :
	UniformRandomIntegerParameter(int low, int high) : _random(low, high) {}

	virtual int operator()(const I & in) {
		return this->_random(this->_rng);
	}
private :
	std::uniform_int_distribution<int> _random;
};

template <typename I>
class UniformRandomDoubleParameter : public RandomParameter<I, double> {
public:
	UniformRandomDoubleParameter(double low, double high) : _random(low, high) {}

	virtual double operator()(const I & in) {
		return this->_random(this->_rng);
	}
private:
	std::uniform_real_distribution<double> _random;
};