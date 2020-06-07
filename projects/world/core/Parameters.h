#pragma once

#include "WorldConfig.h"

#include <random>
#include <time.h>

#include "world/math/MathsHelper.h"
#include "ICloneable.h"

namespace world {

template <typename Out, typename... In> class Parameter {
public:
    void setFunction(std::function<Out(In...)> func) { _function = func; }

    Out operator()(In... in) const { return _function(in...); }

private:
    std::function<Out(In...)> _function;
};

template <typename Out, typename... In> struct Params {
    static std::mt19937 &rng() {
        static std::mt19937 _rng(static_cast<u32>(time(NULL)));
        return _rng;
    };

    static Parameter<Out, In...> constant(Out value) {
        Parameter<Out, In...> ret;
        ret.setFunction([value](In... in) { return value; });
        return ret;
    }

    static Parameter<Out, In...> gaussian(double mean, double deviation) {
        Parameter<Out, In...> ret;
        ret.setFunction([mean, deviation](In... in) {
            std::normal_distribution<Out> distrib(mean, deviation);
            return distrib(rng());
        });
        return ret;
    }

    static Parameter<Out, In...> uniform_int(Out min, Out max) {
        Parameter<Out, In...> ret;
        ret.setFunction([min, max](In... in) {
            std::uniform_int_distribution<Out> distrib(min, max);
            return distrib(rng());
        });
        return ret;
    }

    static Parameter<Out, In...> uniform_real(Out min, Out max) {
        Parameter<Out, In...> ret;
        ret.setFunction([min, max](In... in) {
            std::uniform_real_distribution<Out> distrib(min, max);
            return distrib(rng());
        });
        return ret;
    }

    static Parameter<Out, In...> read(const WorldFile &wf) {
        Parameter<Out, In...> ret;
        // TODO
        return ret;
    }
};
} // namespace world
