#ifndef QTWORLD_H
#define QTWORLD_H

#include <Qt3DCore/QEntity>
#include <qimage.h>

#include <worldapi/mesh.h>
#include <worldapi/assets/Image.h>

class QtWorld
{
public:
    QtWorld() = delete;

    static Qt3DCore::QEntity * getQMesh(const Mesh & mesh, Qt3DCore::QEntity * parent);
    static QImage * getQImage(const Image & image);
};

#endif // QTWORLD_H
