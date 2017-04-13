#include "qtworld.h"

using namespace Qt3DCore;
using namespace img;

QEntity * QtWorld::getQMesh(const Mesh & mesh, QEntity * parent)
{
    QEntity * result = new QEntity(parent);

    return result;
}

QImage * QtWorld::getQImage(const Image & image)
{
    // Détermination du format Qt de l'image.
    ImageType type = image.type();
    QImage::Format format;

    switch (type) {
    case ImageType::RGB:
        format = QImage::Format::Format_RGB32;
        break;
    case ImageType::RGBA:
        format = QImage::Format::Format_ARGB32;
        break;
    case ImageType::GREYSCALE:
        format = QImage::Format::Format_RGB32;
        break;
    }

    int width = (int) image.width();
    int height = (int) image.height();

    // Conversion
    QImage * qimage = new QImage(width, height, format);

    for (int x = 0 ; x < width ; x++) {
        for (int y = 0 ; y < height ; y++) {
            const ConstPixel & pix = image.at(x, y);
            QRgb value = qRgba(pix.getRed(), pix.getGreen(), pix.getBlue(), pix.getAlpha());
            qimage->setPixel(x, y, value);
        }
    }

    return qimage;
}
