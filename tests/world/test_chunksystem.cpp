#include <catch/catch.hpp>

#include <world/core.h>

using namespace world;

class ExplorationSpy;

class SnitchNode : public WorldNode {
public:
    ExplorationSpy &_spy;


    SnitchNode(ExplorationSpy &spy) : _spy(spy) {}

    void collect(ICollector &collector, const IResolutionModel &model,
                 const ExplorationContext &ctx) override;
};

class ExplorationSpy : public IChunkDecorator {
public:
    bool _verbose = false;
    int _chunkCounter = 0;
    std::map<NodeKey, vec3d> _positions;

    void decorate(Chunk &chunk, const ExplorationContext &ctx) override {
        if (_verbose) {
            std::cout << "Exploration of chunk n°" << _chunkCounter
                      << std::endl;
            std::cout << "\tDimensions: " << chunk.getSize()
                      << ", position: " << chunk.getPosition3D() << std::endl;
            std::cout << "\tResolution: min " << chunk.getMinResolution()
                      << ", max " << chunk.getMaxResolution() << std::endl;
        }

        SnitchNode &node = chunk.addChild<SnitchNode>(*this);
        node.setPosition3D(vec3d{0, 0, 0});

        ++_chunkCounter;
    }
};

void SnitchNode::collect(ICollector &collector, const IResolutionModel &model,
                         const ExplorationContext &ctx) {
    vec3d offset = ctx.getOffset();
    ItemKey itemKey = ctx.mutateKey(ItemKeys::defaultKey());
    NodeKey nodeKey = ItemKeys::getLastNode(ItemKeys::getParent(itemKey));
    _spy._positions[nodeKey] = offset;
}

TEST_CASE("GridChunkSystem", "[chunksystem]") {
    GridChunkSystem chunkSystem(1000, 6, 0.5);
    ExplorationSpy &spy = chunkSystem.addDecorator<ExplorationSpy>();

    SECTION("TileSystem config") {
        const TileSystem &ts = chunkSystem.getTileSystem();

        std::stringstream resolutions;

        for (int i = 0; i <= ts._maxLod; ++i) {
            resolutions << ts.getMinResolution(i) << " "
                        << ts.getMaxResolution(i) << std::endl;
        }
        INFO(resolutions.str());

        CHECK(ts.getMaxResolution(0) == Approx(0.5));
    }

    SECTION("Collecting") {
        FirstPersonView fpsView;
        Collector collector;

        chunkSystem.collect(collector, fpsView);

        bool success = true;
        std::stringstream errors;

        for (auto &entry : spy._positions) {
            vec3d goodOffset =
                chunkSystem.getOffset(TileCoordinates(entry.first));
            vec3d realOffset = entry.second;

            if (goodOffset.length(realOffset) != Approx(0)) {
                success = false;
                errors << realOffset << " should be " << goodOffset
                       << std::endl;
            }
        }

        INFO(errors.str());
        CHECK(success);
    }
}