#pragma once

#include "core/WorldConfig.h"

#include <memory>

#include "TreeSkeletton.h"
#include "core/Parameters.h"

namespace world {
	namespace tree {
		typedef Parameter<TreeInfo, double> param_d;
		typedef Parameter<TreeInfo, int> param_i;
		typedef ConstantParameter<TreeInfo, double> const_d;
		typedef ConstantParameter<TreeInfo, int> const_i;
		typedef GaussianParameter<TreeInfo> gaussian;
		typedef UniformRandomIntegerParameter<TreeInfo> uniform_i;
		typedef UniformRandomDoubleParameter<TreeInfo> uniform_d;

		template<typename T>
		class TreeParameter {
		public :
			virtual T operator()(const TreeInfo &info,
								 int divisionCount = 1, double theta = 0, double phi = 0, double weight = 0,
								 double size = 0) = 0;

			virtual TreeParameter<T> *clone() const = 0;
		};

		template<typename T>
		class TreeParameterWrapper : public TreeParameter<T> {
		public :
			template<typename F>
			TreeParameterWrapper(const F &wrapped) : _wrapped(std::make_shared<F>(wrapped)) {}

			virtual T operator()(const TreeInfo &info,
								 int divisionCount, double theta, double phi, double weight, double size) {

				return (*_wrapped)(info);
			}

			virtual TreeParameterWrapper<T> *clone() const {
				return new TreeParameterWrapper<T>(*this);
			}

		private :
			TreeParameterWrapper() : _wrapped(nullptr) {}

			std::shared_ptr<Parameter<TreeInfo, T>> _wrapped;
		};

		template<typename T>
		class TreeParameterChain : public TreeParameter<T> {
		public:
			template<class F>
			TreeParameterChain(const F &chained) : _chained(std::make_shared<F>(chained)) {}

			virtual T operator()(const TreeInfo &info,
								 int divisionCount, double theta, double phi, double weight, double size) {

				return (*_chained)(info, divisionCount, theta, phi, weight, size);
			}

			virtual TreeParameterChain<T> *clone() const {
				return new TreeParameterChain<T>(*this);
			}

		private:
			std::shared_ptr<TreeParameter<T>> _chained;
		};

		typedef TreeParameter<double> tree_param_d;
		typedef TreeParameterWrapper<double> wrapper_d;
		typedef TreeParameterWrapper<int> wrapper_i;
		typedef TreeParameterChain<double> chain_d;
		typedef TreeParameterChain<int> chain_i;

		// POIDS

		class DefaultWeightParameter : public TreeParameter<double> {
		public :
			virtual double operator()(const TreeInfo &info,
									  int divisionCount, double theta, double phi, double weight, double size) {
				return info.getWeight() / divisionCount;
			}

			virtual DefaultWeightParameter *clone() const {
				return new DefaultWeightParameter(*this);
			}
		};

		class SpecialWeightParameter1 : public TreeParameter<double> {
		public :
			virtual double operator()(const TreeInfo &info,
									  int divisionCount, double theta, double phi, double weight, double size) {

				double w = info.getWeight() * 1.5;
				if (info._level == 1) {
					w /= 10.0;
				}
				return w / divisionCount;
			}

			virtual SpecialWeightParameter1 *clone() const {
				return new SpecialWeightParameter1(*this);
			}
		};

		// TAILLE

		class SpecialSizeParameter1 : public gaussian {
		public :
			SpecialSizeParameter1(double median, double deviation)
					: gaussian(median, deviation) {}

			virtual double operator()(const TreeInfo &info) {
				double superResult = gaussian::operator()(info);
				if (info._level >= 2) {
					superResult /= 2.0;
				}
				return superResult;
			}

			virtual SpecialSizeParameter1 *clone() const {
				return new SpecialSizeParameter1(*this);
			}
		};

		// MAX LEVEL

		class MaxLevelByWeightParameter : public param_i {
		public :
			MaxLevelByWeightParameter(double weighThreshold) :
					_weightThreshold(weighThreshold) {}

			virtual int operator()(const TreeInfo &info) {
				if (info.getWeight() <= _weightThreshold) {
					return info._level;
				} else {
					return info._level + 1;
				}
			}

			virtual MaxLevelByWeightParameter *clone() const {
				return new MaxLevelByWeightParameter(*this);
			}

		private :
			double _weightThreshold;
		};

		// TODO Les fonctions qui font des branches intermédiaires... des fois.

		class SideBranchPhiParameter : public chain_d {
		public :
			template<class F>
			SideBranchPhiParameter(const F &wrapped) :
					chain_d(wrapped) {}

			virtual double operator()(const TreeInfo &info,
									  int divisionCount, double theta, double phi, double weight, double size) {

				const Node<TreeInfo> &node = info.getNode();
				double result = chain_d::operator()(info, divisionCount, theta, phi, weight, size);
				if (node.getChildrenOrNeighboursCount() == 0 && info.getWeight() > 0.1) {
					result *= 0.1;
				}
				return result;
			}

			virtual SideBranchPhiParameter *clone() const {
				return new SideBranchPhiParameter(*this);
			}

		private :
		};

		class SideBranchOffsetThetaParameter : public chain_d {
		public :
			template<class F>
			SideBranchOffsetThetaParameter(const F &chained) : chain_d(chained) {}

			virtual double operator()(const TreeInfo &info,
									  int divisionCount, double theta, double phi, double weight, double size) {

				const Node<TreeInfo> &node = info.getNode();
				int id = node.getChildrenOrNeighboursCount();
				double result = chain_d::operator()(info, divisionCount, theta, phi, weight, size);
				if (node.getChildrenOrNeighboursCount() != 0 && info.getWeight() > 0.1) {
					result += (1.0 / (divisionCount - 1) - 1.0 / divisionCount) * id * M_PI * 2.0;
				}

				return result;
			}

			virtual SideBranchOffsetThetaParameter *clone() const {
				return new SideBranchOffsetThetaParameter(*this);
			}
		};

		class SideBranchSizeParameter : public chain_d {
		public :
			template<class F>
			SideBranchSizeParameter(const F &wrapped) : chain_d(wrapped) {}

			virtual double operator()(const TreeInfo &info,
									  int divisionCount, double theta, double phi, double weight, double size) {

				double result = chain_d::operator()(info, divisionCount, theta, phi, weight, size);
				result *= pow(weight / info.getWeight(), 0.3) * 1.4;

				return result;
			}

			virtual SideBranchSizeParameter *clone() const {
				return new SideBranchSizeParameter(*this);
			}
		};

		class SideBranchWeightParameter : public chain_d {
		public :
			template<class F>
			SideBranchWeightParameter(const F &wrapped) :
					chain_d(wrapped) {}

			virtual double operator()(const TreeInfo &info,
									  int divisionCount, double theta, double phi, double weight, double size) {

				double result = chain_d::operator()(info, divisionCount, theta, phi, weight, size);
				if (phi <= M_PI / 36) {
					result = info.getWeight() * pow(0.75, info._level);
				} else {
					result *= 0.6;
				}
				return result;
			}

			virtual SideBranchSizeParameter *clone() const {
				return new SideBranchSizeParameter(*this);
			}
		};
	}
}