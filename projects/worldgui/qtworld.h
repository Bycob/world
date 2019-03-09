#ifndef QTWORLD_H
#define QTWORLD_H

#include <Qt3DCore/QEntity>
#include <qimage.h>

#include <world/core.h>

class QtWorld {
public:
    QtWorld() = delete;

    static Qt3DCore::QEntity *getQMesh(const world::Mesh &mesh,
                                       Qt3DCore::QEntity *parent);
    static QImage *getQImage(const world::Image &image);
};

#endif // QTWORLD_H
