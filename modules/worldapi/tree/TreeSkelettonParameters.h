#pragma once

#include "core/WorldConfig.h"

#include <memory>

#include "TreeSkeletton.h"
#include "core/Parameters.h"

namespace world {

template <typename T> using TreeParam = Parameter<T, TreeInfo, TreeInfo>;
using TreeParamd = TreeParam<double>;
using TreeParami = TreeParam<int>;

template <typename T>
struct TreeParams : public Params<T, TreeInfo, TreeInfo> {};

struct TreeParamsd : public TreeParams<double> {
    static TreeParamd DefaultWeight() {
        TreeParamd param;
        param.setFunction([](TreeInfo parent, TreeInfo child) {
            return parent._weight / parent._forkCount;
        });
        return param;
    }

    /** Creates a basic weight function (each branch receive
     * a part of the weight from the lower branch). Then you can
     * add a function to jitter this weight of a certain amount
     * @param jitter The weight obtained before will be multiplied
     * by this parameter. */
    static TreeParamd SharedWeight(const TreeParamd &jitter) {
        TreeParamd param;
        param.setFunction([jitter](TreeInfo parent, TreeInfo child) {
            return jitter(parent, child) * parent._weight / parent._forkCount;
        });
        return param;
    }

    static TreeParamd UniformTheta(const TreeParamd &jitter) {
        TreeParamd param;
        param.setFunction([jitter](TreeInfo parent, TreeInfo child) {
            return jitter(parent, child) +
                   (2.0 * M_PI * child._forkId) / parent._forkCount;
        });
        return param;
    }

    static TreeParamd PhiOffset(const TreeParamd &offset) {
        TreeParamd param;
        param.setFunction([offset](TreeInfo parent, TreeInfo child) {
            return offset(parent, child) +
                   cos(parent._theta - child._theta) * parent._phi;
        });
        return param;
    }

    static TreeParamd SizeFactor(const TreeParamd &reductionFactor) {
        TreeParamd param;
        param.setFunction([reductionFactor](TreeInfo parent, TreeInfo child) {
            return reductionFactor(parent, child) * parent._size;
        });
        return param;
    }

    static TreeParamd SizeByWeight(double density) {
        TreeParamd param;
        param.setFunction([density](TreeInfo parent, TreeInfo child) {
            return pow(child._weight / density, 0.5);
        });
        return param;
    }
};

struct TreeParamsi : public TreeParams<int> {
    static TreeParami WeightThreshold(double weightThreshold,
                                      const TreeParami &forkCount) {
        TreeParami param;
        param.setFunction([forkCount, weightThreshold](TreeInfo parent,
                                                       TreeInfo child) {
            return child._weight >= weightThreshold ? forkCount(parent, child)
                                                    : 0;
        });
        return param;
    }
};


struct SideBranchd {
    static TreeParamd Weight(const TreeParamd &factor) {
        TreeParamd param;
        param.setFunction([factor](TreeInfo parent, TreeInfo child) {
            double compFactor = factor(parent, child);

            if (child._forkId == 0) {
                return parent._weight *
                       (1 + (1 - compFactor) * (parent._forkCount - 1)) /
                       parent._forkCount;
            } else {
                return parent._weight * compFactor / parent._forkCount;
            }
        });
        return param;
    }

    static TreeParamd Theta(const TreeParamd &jitter) {
        TreeParamd param;
        param.setFunction([jitter](TreeInfo parent, TreeInfo child) {
            if (child._forkId == 0)
                return parent._theta;

            int i = child._forkId - 1;
            int c = parent._forkCount - 1;

            return jitter(parent, child) + (2.0 * M_PI * i) / c;
        });
        return param;
    }

    static TreeParamd Phi(const TreeParamd &sideBranchOffset) {
        TreeParamd param;
        TreeParamd sideBranchPhi = TreeParamsd::PhiOffset(sideBranchOffset);
        param.setFunction([sideBranchPhi](TreeInfo parent, TreeInfo child) {
            if (child._forkId == 0)
                return parent._phi;
            else
                return sideBranchPhi(parent, child);
        });
        return param;
    }

    static TreeParamd Size(const TreeParamd &nextBranchDistance) {
        TreeParamd param;
        TreeParamd sizeByWeight = TreeParamsd::SizeByWeight(1);
        param.setFunction([nextBranchDistance, sizeByWeight](TreeInfo parent,
                                                             TreeInfo child) {
            double baseSize = sizeByWeight(parent, child);

            if (child._forkId == 0 && parent._forkCount != 1) {
                return baseSize * nextBranchDistance(parent, child);
            } else {
                return baseSize;
            }
        });
        return param;
    }
};

/*
        // POIDS

        class SpecialWeightParameter1 : public TreeParameter<double> {
        public :
                virtual double operator()(const TreeInfo &info,
                                                                  int
   divisionCount, double theta, double phi, double weight, double size) {

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
*/

} // namespace world