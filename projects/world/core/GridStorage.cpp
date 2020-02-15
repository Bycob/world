#include "GridStorage.h"

namespace world {

void GridStorageBase::setReducer(GridStorageReducer *reducer) {
    _reducer = reducer;
    _reducer->registerStorage(this);
}

} // namespace world
