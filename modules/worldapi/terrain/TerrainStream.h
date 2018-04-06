#ifndef WORLD_TERRAINSTREAM_H
#define WORLD_TERRAINSTREAM_H

#include "core/WorldConfig.h"

#include <ios>

#include "Terrain.h"

namespace world {

    enum class HeightMapFormat {
        F32,
        F64,
        U8
    };

    class WORLDAPI_EXPORT HeightMapInputStream {
    public:
        HeightMapInputStream(const Terrain &terrain, double offset = 0, double scale = 0, HeightMapFormat format = HeightMapFormat::F32);

        HeightMapInputStream(const HeightMapInputStream &stream);

        void setOffset(double offset);

        void setScale(double scale);

        void setOutputFormat(HeightMapFormat format);

        int remaining() const;

        int read(char* buffer, int count);
    private:
        double _offset;
        double _scale;
        HeightMapFormat _format;
        const Terrain &_terrain;

        int _position = 0;

        int getTotalSize() const;
    };
}

#endif //WORLD_TERRAINSTREAM_H
