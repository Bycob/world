#pragma once

#include "core/WorldConfig.h"

#include <memory>

#include "TreeSkeletton.h"
#include "core/Parameters.h"

namespace world {

	template <typename T>
	using TreeParam = Parameter<T, TreeInfo, int, double, double, double, double>;
	using TreeParamd = TreeParam<double>;
	using TreeParami = TreeParam<int>;

	template <typename T>
	struct TreeParams : public Params<T, TreeInfo, int, double, double, double, double> {

	};

	struct TreeParamsd : public TreeParams<double> {
		static TreeParamd DefaultWeight() {
			TreeParamd param;
			param.setFunction([] (TreeInfo parent,
								  int divisionCount, double theta, double phi, double weight, double size) {
				return parent._weight / divisionCount;
			});
			return param;
		}
	};

	struct TreeParamsi : public TreeParams<int> {
        static TreeParami MaxLevelByWeight(double weightThreshold) {
            TreeParami param;
            param.setFunction([weightThreshold] (TreeInfo info,
                                                 int divisionCount, double theta, double phi, double weight, double size) {

                if (info._weight <= weightThreshold) {
                    return info._level;
                } else {
                    return info._level + 1;
                }
            });
            return param;
        };
	};
/*
	// POIDS

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
*/

	struct SideBranchd {
		// TODO Les fonctions qui font des branches intermédiaires... des fois.
	};
/*
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
	*/
}