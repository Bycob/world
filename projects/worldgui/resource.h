#ifndef RESOURCE_H
#define RESOURCE_H

#include <memory>

#include <QObject>

#include <worldcore.h>

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
    MeshResource(QString name, world::Scene * scene);

    virtual void save(QString path);

private:
    world::Scene* _scene;
};

class ImageResource : public Resource {
public:
    ImageResource(QString name, world::Image * image);

    virtual void save(QString path);
private:
    world::Image * _image;
};

#endif // RESOURCE_H
