#include "resource.h"

Resource::Resource(QString name) : _name(name)
{

}

QString Resource::name() const {
    return _name;
}


MeshResource::MeshResource(QString name, world::Scene *scene)
    : Resource(name), _scene(scene) {

}

void MeshResource::save(QString path) {
    world::ObjLoader loader;
    loader.write(*_scene, path.toStdString());
}




ImageResource::ImageResource(QString name, world::Image *image)
    : Resource(name), _image(image) {

}

void ImageResource::save(QString path) {
    _image->write(path.toStdString());
}
