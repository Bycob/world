#ifndef QTWORLD_H
#define QTWORLD_H

#include <Qt3DCore/QEntity>
#include <worldapi/files/mesh.h>

class QtWorld
{
public:
    QtWorld() = delete;

    static Qt3DCore::QEntity * toQt(const Mesh & mesh, Qt3DCore::QEntity * parent);
};

#endif // QTWORLD_H
