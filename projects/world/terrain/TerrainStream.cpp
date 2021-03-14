#include "TerrainStream.h"

#include "world/core/WorldTypes.h"
#include "world/math/MathsHelper.h"

namespace world {
int getSize(HeightMapFormat format) {
    switch (format) {
    case HeightMapFormat::F32:
        return sizeof(f32);
    case HeightMapFormat::F64:
        return sizeof(f64);
    case HeightMapFormat::U8:
        return sizeof(u8);
    }
    throw std::runtime_error("Unknown HeightmapFormat: " + std::to_string(
        static_cast<int>(format)));
}

HeightMapInputStream::HeightMapInputStream(const world::Terrain &terrain,
                                           double offset, double scale,
                                           world::HeightMapFormat format)
        : _offset(offset), _scale(scale), _format(format), _terrain(terrain) {}

HeightMapInputStream::HeightMapInputStream(
    const world::HeightMapInputStream &stream)
        : _terrain(stream._terrain) {}

void HeightMapInputStream::setOffset(double offset) { _offset = offset; }

void HeightMapInputStream::setScale(double scale) { _scale = scale; }

void HeightMapInputStream::setOutputFormat(world::HeightMapFormat format) {
    _format = format;
}

int HeightMapInputStream::remaining() const {
    return (getTotalSize() - _position) * getSize(_format);
}

int HeightMapInputStream::read(char *buffer, int count) {
    const int fsize = getSize(_format);
    count /= fsize;
    const int res = _terrain.getResolution();

    for (int i = 0; i < count; i++) {
        if (remaining() == 0) {
            return i * fsize;
        }

        // COLUMN MAJOR ORDER
        int y = _position % res;
        int x = (_position - y) / res;
        double data = _terrain(x, y) * _scale + _offset;

        switch (_format) {
        case HeightMapFormat::F32:
            (reinterpret_cast<f32 *>(buffer))[i] = static_cast<f32>(data);
            break;
        case HeightMapFormat::F64:
            (reinterpret_cast<f64 *>(buffer))[i] = static_cast<f64>(data);
            break;
        case HeightMapFormat::U8:
            buffer[i] = static_cast<u8>(clamp(data, 0, 1) * 255);
            break;
        }

        _position++;
    }

    return count * fsize;
}

int HeightMapInputStream::getTotalSize() const {
    const int res = _terrain.getResolution();
    return res * res;
}
} // namespace world
