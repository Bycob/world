#ifndef WORLD_BINARYSTREAM_H
#define WORLD_BINARYSTREAM_H

#include "core/WorldConfig.h"

#include <iostream>

namespace world {

    class bin_ostream {
    public:
        bin_ostream(std::ostream &stream);

        bin_ostream &operator<<(const double &value);

        bin_ostream &operator<<(const float &value);

    private:
        std::ostream &_stream;
    };

    class bin_istream {
    public:
        bin_istream(std::istream &stream);

    private:
        std::istream &_stream;
    };
}

#endif //WORLD_BINARYSTREAM_H
