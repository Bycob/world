#pragma once

#include "WorldConfig.h"

#include <random>
#include <time.h>

#include "math/MathsHelper.h"
#include "ICloneable.h"

namespace world {

//FONCTIONS-PARAMETRES

	template<typename I, typename T>
	class Parameter : public ICloneable<Parameter<I, T>> {
	public:
		virtual T operator()(const I &in) = 0;

		virtual Parameter<I, T> *clone() const = 0;
	};

	template<typename I, typename T>
	class ConstantParameter : public Parameter<I, T> {
	public :
		ConstantParameter(T value) : _value(value) {}

		virtual T operator()(const I &in) {
			return _value;
		}

		virtual ConstantParameter<I, T> *clone() const {
			return new ConstantParameter<I, T>(*this);
		}

	private :
		T _value;
	};

	template<typename I, typename T>
	class RandomParameter : public Parameter<I, T> {
	public :
		RandomParameter() : _rng(time(NULL)) {}

	protected :
		std::mt19937 _rng;
	};

	template<typename I>
	class GaussianParameter : public RandomParameter<I, double> {
	public :
		GaussianParameter(double median, double deviation) : _random(median, deviation) {}

		virtual double operator()(const I &in) {
			return this->_random(this->_rng);
		}

		virtual GaussianParameter<I> *clone() const {
			return new GaussianParameter<I>(*this);
		}

	private :
		std::normal_distribution<double> _random;
	};

	template<typename I>
	class UniformRandomIntegerParameter : public RandomParameter<I, int> {
	public :
		UniformRandomIntegerParameter(int low, int high) : _random(low, high) {}

		virtual int operator()(const I &in) {
			return this->_random(this->_rng);
		}

		virtual UniformRandomIntegerParameter<I> *clone() const {
			return new UniformRandomIntegerParameter<I>(*this);
		}

	private :
		std::uniform_int_distribution<int> _random;
	};

	template<typename I>
	class UniformRandomDoubleParameter : public RandomParameter<I, double> {
	public:
		UniformRandomDoubleParameter(double low, double high) : _random(low, high) {}

		virtual double operator()(const I &in) {
			return this->_random(this->_rng);
		}

		virtual UniformRandomDoubleParameter<I> *clone() const {
			return new UniformRandomDoubleParameter<I>(*this);
		}

	private:
		std::uniform_real_distribution<double> _random;
	};
}
