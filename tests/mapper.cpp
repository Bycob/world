#include <world/core.h>
#include <world/flat.h>
#include <world/terrain.h>

using namespace world;

int main(int argc, char **argv) {
    std::unique_ptr<FlatWorld> world;

    if (argc > 1) {
        world = std::make_unique<FlatWorld>();
        world->load(argv[1]);
    } else {
        world.reset(FlatWorld::createDemoFlatWorld());
    }

    FlatMapper mapper;
    Image output(1024, 1024, ImageType::RGB);

    double u = 50000;

    mapper.map(*world, output, {{-u, -u, -2000}, {u, u, 4000}});
    output.write("map.png");

    // Export biomes
    auto &ground = dynamic_cast<HeightmapGround &>(world->ground());
    auto &gb = ground.getWorker<GroundBiomes>();

    Image img(1024, 1024, ImageType::RGBA);
    gb.exportZones(img, BoundingBox({-u}, {u}), 0);
    img.write("zones.png");
    std::cout << "Wrote zones" << std::endl;
}