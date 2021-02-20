#ifndef WORLD_GROUNDTEXTUREGENERATOR_H
#define WORLD_GROUNDTEXTUREGENERATOR_H

#include "world/core/WorldConfig.h"

#include <map>
#include <utility>

#include "GroundBiomes.h"
#include "MultilayerGroundTexture.h"

namespace world {

    /** Generates monochromatic textures. */
    class WORLDAPI_EXPORT GroundTextureGenerator
        : public ITextureProvider, public IBiomeTextureGenerator {

        WORLD_WRITE_SUBCLASS_METHOD
    public:
        GroundTextureGenerator(u32 texWidth = 256);

        Image &getTexture(int layer, int lod) override;

        void addLayer(const Color4d &color);

        void addLayer(const BiomeLayer &layer) override;

        void write(WorldFile &wf) const override;

        void read(const WorldFile &wf) override;

    private:
        std::map<std::pair<int, int>, Image> _images;
        std::vector<Color4d> _colors;

        u32 _texWidth;
    };

} // namespace world

#endif // WORLD_GROUNDTEXTUREGENERATOR_H
