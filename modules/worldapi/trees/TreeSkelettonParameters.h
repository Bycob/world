#pragma once

#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>

#include "TreeSkeletton.h"
#include "../GenParametersUtil.h"

namespace treegen {
	typedef ParameterFunc<TreeInfo, double> param_d;
	typedef ParameterFunc<TreeInfo, int> param_i;
	typedef ConstantParameterFunc<TreeInfo, double> const_d;
	typedef ConstantParameterFunc<TreeInfo, int> const_i;
	typedef GaussianParameterFunc<TreeInfo> gaussian;
	typedef UniformIntegerParameterFunc<TreeInfo> uniform_i;
	typedef UniformDoubleParameterFunc<TreeInfo> uniform_d;

	template <typename T>
	class TreeParameterFunc {
	public :
		virtual T operator()(const TreeInfo & info,
			int divisionCount = 1, double theta = 0, double phi = 0, double weight = 0, double size = 0) = 0;
	};

	template <typename T>
	class TreeParameterWrapper : public TreeParameterFunc<T> {
	public :
		template <typename F>
		TreeParameterWrapper(const F & wrapped) : _wrapped(std::make_shared<F>(wrapped)) {}

		virtual T operator()(const TreeInfo & info,
			int divisionCount, double theta, double phi, double weight, double size) {

			return (*_wrapped)(info);
		}
	private :
		TreeParameterWrapper() : _wrapped(nullptr) {}

		std::shared_ptr<ParameterFunc<TreeInfo, T>> _wrapped;
	};

	template <typename T>
	class TreeParameterChain : public TreeParameterFunc<T> {
	public:
		template <class F>
		TreeParameterChain(const F & chained) : _chained(std::make_shared<F>(chained)) {}

		virtual T operator()(const TreeInfo & info,
			int divisionCount, double theta, double phi, double weight, double size) {

			return (*_chained)(info, divisionCount, theta, phi, weight, size);
		}
	private:
		std::shared_ptr<TreeParameterFunc<T>> _chained;
	};

	typedef TreeParameterFunc<double> tree_param_d;
	typedef TreeParameterWrapper<double> wrapper_d;
	typedef TreeParameterWrapper<int> wrapper_i;
	typedef TreeParameterChain<double> chain_d;
	typedef TreeParameterChain<int> chain_i;

	// POIDS

	class DefaultWeightFunc : public TreeParameterFunc<double> {
	public :
		virtual double operator()(const TreeInfo & info,
			int divisionCount, double theta, double phi, double weight, double size) {
			return info.getWeight() / divisionCount;
		}
	};

	class SpecialWeightFunc1 : public TreeParameterFunc<double> {
	public :
		virtual double operator()(const TreeInfo & info,
			int divisionCount, double theta, double phi, double weight, double size) {
			
			double w = info.getWeight() * 1.5;
			if (info._level == 1) {
				w /= 10.0;
			}
			return w / divisionCount;
		}
	};

	// TAILLE

	class SpecialSizeFunc1 : public gaussian {
	public :
		SpecialSizeFunc1(double median, double deviation)
			: gaussian(median, deviation) {}

		virtual double operator()(const TreeInfo & info) {
			double superResult = gaussian::operator()(info);
			if (info._level >= 2) {
				superResult /= 2.0;
			}
			return superResult;
		}
	};

	// MAX LEVEL

	class MaxLevelByWeightFunc : public param_i {
	public :
		MaxLevelByWeightFunc(double weighThreshold) :
			_weightThreshold(weighThreshold) {}

		virtual int operator()(const TreeInfo & info) {
			if (info.getWeight() <= _weightThreshold) {
				return info._level;
			}
			else {
				return info._level + 1;
			}
		}

	private :
		double _weightThreshold;
	};

	// TODO Les fonctions qui font des branches intermédiaires... des fois.

	class SideBranchPhiFunc : public chain_d {
	public :
		template <class F>
		SideBranchPhiFunc(const F & wrapped) : 
			chain_d(wrapped) {}

		virtual double operator()(const TreeInfo & info,
			int divisionCount, double theta, double phi, double weight, double size) {

			const Node<TreeInfo> &node = info.getNode();
			double result = chain_d::operator()(info, divisionCount, theta, phi, weight, size);
			if (node.getChildrenOrNeighboursCount() == 0 && info.getWeight() > 0.1) {
				result *= 0.1;
			}
			return result;
		}
	private :
	};

	class SideBranchOffsetTheta : public chain_d {
	public :
		template <class F>
		SideBranchOffsetTheta(const F & chained) : chain_d(chained) {}

		virtual double operator()(const TreeInfo & info,
			int divisionCount, double theta, double phi, double weight, double size) {

			const Node<TreeInfo> &node = info.getNode();
			int id = node.getChildrenOrNeighboursCount();
			double result = chain_d::operator()(info, divisionCount, theta, phi, weight, size);
			if (node.getChildrenOrNeighboursCount() != 0 && info.getWeight() > 0.1) {
				result += (1.0 / (divisionCount - 1) - 1.0 / divisionCount) * id * M_PI * 2.0;
			}

			return result;
		}
	};

	class SideBranchSizeFunc : public chain_d {
	public :
		template <class F>
		SideBranchSizeFunc(const F & wrapped) : chain_d(wrapped) {}

		virtual double operator()(const TreeInfo & info,
			int divisionCount, double theta, double phi, double weight, double size) {

			double result = chain_d::operator()(info, divisionCount, theta, phi, weight, size);
			result *= pow(weight / info.getWeight(), 0.3) * 1.4;

			return result;
		}
	};
	
	class SideBranchWeightFunc : public chain_d {
	public :
		template <class F>
		SideBranchWeightFunc(const F & wrapped) :
			chain_d(wrapped) {}

		virtual double operator()(const TreeInfo & info,
			int divisionCount, double theta, double phi, double weight, double size) {

			double result = chain_d::operator()(info, divisionCount, theta, phi, weight, size);
			if (phi <= M_PI / 36) {
				result = info.getWeight() * pow(0.75, info._level);
			}
			else {
				result *= 0.6;
			}
			return result;
		}
	};
}