#include "ProxyGround.h"
#include "ProxyGround_p.h"

#include <set>

#include <world/assets/Image.h>

#include "Vulkan_p.h"

namespace world {

ProxyGround::ProxyGround(f64 width, u32 resolution) {
    const u32 tileSize = 128;
    const u32 tileCount = resolution / tileSize;
    _internal = new ProxyGroundPrivate(width / tileCount, tileSize, tileCount);
}

ProxyGround::~ProxyGround() { delete _internal; }

void ProxyGround::collect(ICollector &collector,
                          const IResolutionModel &resolutionModel) {
    if (!collector.hasChannel<Image>())
        return;

    auto &imageChan = collector.getChannel<Image>();

    auto &vkctx = Vulkan::context().internal();

    // ComputePipeline p1("upscale");
    // p1.addBinding(DescriptorType::STORAGE_BUFFER, 0);
    // p1.addBinding(DescriptorType::UNIFORM_BUFFER, 1);

    // ComputePipeline p2("derivation", layout1);

    auto &tileSystem = _internal->_tileSystem;
    u32 bufferSize = tileSystem._bufferRes.x * tileSystem._bufferRes.y;

    BoundingBox bbox; // TODO
    std::set<TileCoordinates> coords;

    for (auto it = _internal->_tileSystem.iterate(resolutionModel, bbox);
         !it.endReached(); ++it) {
        auto &tileCoords = *it;
        coords.insert(tileCoords);
    }

    std::set<TileCoordinates> toAdd;

    for (const TileCoordinates &tc : coords) {
        auto &tileData = getData(tc);

        tileData._vkData = std::make_unique<ProxyGroundDataPrivate::VkData>(
            ProxyGroundDataPrivate::VkData{
                vkctx.allocate(bufferSize, DescriptorType::STORAGE_BUFFER,
                               MemoryType::CPU_READS),
                vkctx.allocate(bufferSize, DescriptorType::STORAGE_BUFFER,
                               MemoryType::CPU_READS)});


        // DescriptorSet dset;
        // dset.addBuffer(0, tileData._vkData->height);
        // ...

        // Worker &worker = _tileData._worker;
        // worker.bindCommand(dset);
        // worker.dispatchCommand(p1);
        // worker.run();
    }

    for (const TileCoordinates &tc : coords) {
        auto &tileData = getData(tc);

        // tileData._worker.waitCompletion();
        // Image img = tileData._resultBuffer.readData();

        ItemKey key = ItemKeys::inObject(
            tc._pos.x + 0xFFFF * tc._pos.y); // TODO better id ?
                                             // _tileSystem.getId() ?
                                             // imageChan.put(key, img);
    }
}

ProxyGroundDataPrivate &ProxyGround::getData(
    const TileCoordinates &tileCoords) {
    auto &found = _internal->_data.find(tileCoords);
    if (found == _internal->_data.end()) {
        return *(_internal->_data[tileCoords] =
                     std::make_unique<ProxyGroundDataPrivate>());
    } else {
        return *found->second;
    }
}

} // namespace world
