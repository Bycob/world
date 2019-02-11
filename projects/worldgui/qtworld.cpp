#include "qtworld.h"

using namespace Qt3DCore;


QEntity *QtWorld::getQMesh(const world::Mesh &mesh, QEntity *parent) {
    QEntity *result = new QEntity(parent);

    return result;
}

QImage *QtWorld::getQImage(const world::Image &image) {
    // Détermination du format Qt de l'image.
    world::ImageType type = image.type();
    QImage::Format format;

    switch (type) {
    case world::ImageType::RGB:
        format = QImage::Format::Format_RGB32;
        break;
    case world::ImageType::RGBA:
        format = QImage::Format::Format_ARGB32;
        break;
    case world::ImageType::GREYSCALE:
        format = QImage::Format::Format_RGB32;
        break;
    }

    int width = (int)image.width();
    int height = (int)image.height();

    // Conversion
    QImage *qimage = new QImage(width, height, format);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            auto &pix = image.rgb(x, y);
            QRgb value =
                qRgba(pix.getRed(), pix.getGreen(), pix.getBlue(), 255);
            qimage->setPixel(x, y, value);
        }
    }

    return qimage;
}
