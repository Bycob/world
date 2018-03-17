#include "resource.h"

Resource::Resource(QString name) : _name(name)
{

}

QString Resource::name() const {
    return _name;
}


MeshResource::MeshResource(QString name, Scene *scene)
    : Resource(name), _scene(scene) {

}

void MeshResource::save(QString path) {
    _scene->save(path);
}




ImageResource::ImageResource(QString name, Image *image)
    : Resource(name), _image(image) {

}

void ImageResource::save(QString path) {
    _image->write(path.toStdString());
}
