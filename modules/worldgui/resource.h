#ifndef RESOURCE_H
#define RESOURCE_H

#include <memory>

#include <QObject>

#include <worldapi/Image.h>

#include "scene.h"

class Resource
{
public:
    Resource(QString name);

    QString name() const;

    virtual void save(QString path) = 0;

protected:
    QString _name;
};

class MeshResource : public Resource {
public:
    MeshResource(QString name, Scene * scene);

    virtual void save(QString path);

private:
    Scene* _scene;
};

class ImageResource : public Resource {
public:
    ImageResource(QString name, img::Image * image);

    virtual void save(QString path);
private:
    img::Image * _image;
};

#endif // RESOURCE_H
