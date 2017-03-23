#include "qtworld.h"

using namespace Qt3DCore;

QEntity * QtWorld::toQt(const Mesh & mesh, QEntity * parent)
{
    QEntity * result = new QEntity(parent);

    return result;
}
