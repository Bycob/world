#include <world/core.h>
#include <world/flat.h>

using namespace world;

int main(int argc, char **argv) {
    std::unique_ptr<FlatWorld> world(FlatWorld::createDemoFlatWorld());

    FlatMapper mapper;
    Image output(1024, 1024, ImageType::RGB);

    double u = 50000;

    mapper.map(*world, output, {{-u, -u, -2000}, {u, u, 4000}});
    output.write("map.png");
}