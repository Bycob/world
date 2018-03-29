#include "LODGridChunkSystem.h"

#include <string>
#include <map>
#include <vector>

namespace world {

    using QueryResult = IChunkSystem::QueryResult;

    class ChunkEntry {
    public:
        template<typename... Args>
        ChunkEntry(ChunkKey parent, const LODGridCoordinates &coords, Args...args)
                : _parentID(parent), _coords(coords), _chunk(args...) {}

        LODGridCoordinates _coords;
        Chunk _chunk;
        ChunkKey _parentID;
        std::vector<ChunkKey> _children;
    };

    class LODGridChunkSystem::Impl {
    public:
        std::map<int, LODData> _lodData;
        std::map<ChunkKey, std::unique_ptr<ChunkEntry>> _chunks;
    };

    LODGridChunkSystem::LODGridChunkSystem() : _internal(new Impl()) {
        _internal->_lodData.emplace(0, LODData({4000, 4000, 4000}));

        for (int i = 1; i <= _maxLOD; i++) {
            getOrCreateLODData(i);
        }
    }

    LODGridChunkSystem::~LODGridChunkSystem() {
        delete _internal;
    }

    void LODGridChunkSystem::setLODData(int lod, const LODData &data) {
        _internal->_lodData[lod] = data;
    }

    LODData &LODGridChunkSystem::getLODData(int lod) const {
        return _internal->_lodData.at(lod);
    }

    double LODGridChunkSystem::getMaxDetailSize(int lod) const {
        if (lod == 0) {
            return 1e100;
        }

        auto data = getLODData(lod - 1);
        return data.getMinDetailSize();
    }

    double LODGridChunkSystem::getMinDetailSize(int lod) const {
        if (lod == _maxLOD) {
            return 0;
        }

        auto data = getLODData(lod);
        return data.getMinDetailSize();
    }

    int LODGridChunkSystem::getLODForDetailSize(double detailSize) const {
        int lod = 0;

        for (; lod < _maxLOD; lod++) {
            double min = getMinDetailSize(lod);
            double max = getMaxDetailSize(lod);

            if (detailSize >= min && detailSize < max) {
                break;
            } else if (detailSize >= max && lod == 0) {
                break;
            }
        }

        return lod;
    }

    LODData &LODGridChunkSystem::getOrCreateLODData(int lod) {
        auto lodData = _internal->_lodData.find(lod);

        if (lodData == _internal->_lodData.end()) {
            auto dimensions = _internal->_lodData[0].getChunkSize() * powi(2., -lod);
            return _internal->_lodData[lod] = LODData(dimensions);
        } else {
            return (*lodData).second;
        }
    }

    QueryResult LODGridChunkSystem::getChunk(const vec3d &position) {
        LODData &data = _internal->_lodData[0];
        vec3d intPos = position / data.getChunkSize();

        LODGridCoordinates coords = LODGridCoordinates(
                (int) floor(intPos.x),
                (int) floor(intPos.y),
                (int) floor(intPos.z), 0);

        auto id = createChunk(ChunkKeys::none(), coords);
        return {getZone(id.first), id.second};
    }

    QueryResult
    LODGridChunkSystem::getNeighbourChunk(const WorldZone &chunk, const vec3i &direction) {
        // TODO avoid creating the same chunk twice (if we call getOrCreateNeighbour wrongly)
        ChunkEntry &entry = *_internal->_chunks[chunk->getID()];
        LODGridCoordinates coords = entry._coords;
        LODGridCoordinates ncoords = LODGridCoordinates(coords.getPosition3D() + direction, coords.getLOD());

        auto id = createChunk(entry._parentID, ncoords);
        return {getZone(id.first), id.second};
    }

    std::vector<QueryResult> LODGridChunkSystem::getChildren(const WorldZone &zone) {
        std::vector<QueryResult> vector;

        ChunkEntry &entry = *_internal->_chunks[zone->getID()];
        bool created = false;

        // Create children
        if (entry._children.empty() && entry._coords.getLOD() < _maxLOD) {
            int lod = entry._coords.getLOD() + 1;

            // Actuellement, a chaque fois qu'on monte d'un lod, on divise la taille du chunk par 2
            int factor = 2;

            for (int x = 0; x < factor; x++) {
                for (int y = 0; y < factor; y++) {
                    for (int z = 0; z < factor; z++) {
                        LODGridCoordinates ncoords(x, y, z, lod);
                        auto id = createChunk(zone->getID(), ncoords);
                        entry._children.push_back(id.first);
                    }
                }
            }

            created = true;
        }

        std::transform(entry._children.begin(),
                       entry._children.end(),
                       std::inserter(vector, vector.end()), [&](const ChunkKey &id) {

                    return QueryResult { getZone(id), created };
                });

        return vector;
    }

    ChunkKey LODGridChunkSystem::getChunkKey(const ChunkKey &parent, const LODGridCoordinates &coords) const {
        std::stringstream stream;
		stream << parent;
		stream << coords.uid();
        return stream.str();
    }

    Chunk &LODGridChunkSystem::getChunk(const std::string &id) {
        return _internal->_chunks[id]->_chunk;
    }

    WorldZone LODGridChunkSystem::getZone(const std::string &id) {
        return WorldZone(new LODGridChunkHandler(*this, id, getChunk(id)));
    }

    std::pair<ChunkKey, bool> LODGridChunkSystem::createChunk(const ChunkKey &parent, const LODGridCoordinates &coords) {
        auto id = getChunkKey(parent, coords);
        auto chunk = _internal->_chunks.find(id);

        if (chunk == _internal->_chunks.end()) {
            // Get the size of the newly created chunk
            int lod = coords.getLOD();
            LODData &lodData = getOrCreateLODData(lod);
            auto &chunkSize = lodData.getChunkSize();

            // Create the chunk
            auto entry = std::make_unique<ChunkEntry>(parent, coords, coords.getPosition3D() * chunkSize, chunkSize);
            entry->_chunk.setDetailSizeBounds(getMinDetailSize(lod), getMaxDetailSize(lod));

            // Add it to the map
            _internal->_chunks[id] = std::move(entry);
            return {id, true};
        } else {
            return {id, false};
        }
    }


    LODGridChunkHandler::LODGridChunkHandler(LODGridChunkSystem &system, const std::string &id, Chunk &chunk)
            : _system(system), _id(id), _chunk(chunk) {}

    LODGridChunkHandler::LODGridChunkHandler(const LODGridChunkHandler &other)
            : _system(other._system), _id(other._id), _chunk(other._chunk) {}

    IWorldZoneHandler *LODGridChunkHandler::clone() const {
        return new LODGridChunkHandler(*this);
    }

    Chunk &LODGridChunkHandler::chunk() {
        return _chunk;
    }

    const Chunk &LODGridChunkHandler::getChunk() const {
        return _chunk;
    }

    const std::string &LODGridChunkHandler::getID() const {
        return _id;
    }

    bool LODGridChunkHandler::operator<(const LODGridChunkHandler &other) const {
        return _id < other._id;
    }

    bool LODGridChunkHandler::operator==(const LODGridChunkHandler &other) const {
        return _id == other._id;
    }

    bool LODGridChunkHandler::hasParent() const {
        auto &entry = _system._internal->_chunks[_id];
        return entry->_parentID != ChunkKeys::none();
    }

    WorldZone LODGridChunkHandler::getParent() const {
        auto &entry = _system._internal->_chunks[_id];
        return _system.getZone(entry->_parentID);
    }

    vec3d LODGridChunkHandler::getRelativeOffset(const LODGridChunkHandler &other) {
        // TODO

        // We look for the nearest common ancestor between the two
        // We remember the offset of each with their respective ancestors
        /*
        if (*this == other) {
            return {0, 0, 0};
        }

        vec3d thisOffset;
        vec3d otherOffset;

        WorldZone thisCurrent(*this);
        WorldZone otherCurrent(other);

        std::vector<std::unique_ptr<WorldZone>> thisChain;
        std::vector<std::unique_ptr<WorldZone>> otherChain;

        bool ancestorFound = false;

        while(!ancestorFound && (thisCurrent.hasParent() && otherCurrent.hasParent())) {
            if (thisCurrent.hasParent()) {
                thisCurrent = thisCurrent.getParent();

                if (std::find_if(otherChain.begin(), otherChain.end(),
                                 [] (const std::unique_ptr<WorldZone> & zone) {
                                     return *zone == thisCurrent;
                                 }) != otherChain.end()) {

                    ancestorFound = true;
                }
            }
        }*/
        return {};
    }
}
