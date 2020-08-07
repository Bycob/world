#include "TreeSkeletton.h"

#include <iomanip>

namespace world {

std::ostream &operator<<(std::ostream &os, TreeInfo &ti) {
    os << "{" << std::endl;

    // Curve
    os << "\tcurve: "
       << "{" << ti._curve._pts[0];
    for (int i = 1; i < 4; ++i) {
        os << ", " << ti._curve._pts[i];
    }
    os << "}" << std::endl;

    os << "\tweight = " << ti._weight << ", self weight = " << ti._selfWeight;
    os << " (" /* << std::fixed << std::setprecision(1) */
       << ti._selfWeight / ti._weight * 100 << "%)" << std::endl;
    os << "\tsection: " << ti._startArea << " -> " << ti._endArea << std::endl;
    os << "\t" << (ti._connected ? "connected" : "not connected") << ", ";
    os << (ti._hasNext ? "not terminated" : "terminated") << std::endl;
    os << "\tt = " << ti._t << std::endl;
    os << "\ttheta = " << ti._theta << std::endl;
    os << "\tphi = " << ti._phi << std::endl;
    os << "\tsize = " << ti._size << std::endl;
    os << "\tstage " << ti._forkCount << ", id " << ti._forkId << std::endl;
    os << "}" << std::endl;
}

} // namespace world