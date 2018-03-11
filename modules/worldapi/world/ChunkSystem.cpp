#include "ChunkSystem.h"

#include <map>


using namespace maths;

ChunkID::ChunkID(int x, int y, int z, int lod)
        : _pos(x, y, z), _lod(lod) {

}

ChunkID::ChunkID(const maths::vec3i & pos, int lod)
        : _pos(pos), _lod(lod) {

}

ChunkID::ChunkID(const ChunkID & other)
        : _pos(other._pos), _lod(other._lod) {

}

ChunkID::~ChunkID() {

}

const vec3i & ChunkID::getPosition3D() const {
    return _pos;
}

bool ChunkID::operator<(const ChunkID & other) const {
    return _lod < other._lod ? true :
            _lod == other._lod && _pos < other._pos;
}

bool ChunkID::operator==(const ChunkID &other) const {
    return _lod == other._lod && _pos == other._pos;
}


class ChunkEntry {
public:
    template <typename... Args>
    ChunkEntry(ChunkID parent, Args...args)
            : _parentID(parent), _chunk(args...) {}

    Chunk _chunk;
    ChunkID _parentID;
    std::vector<ChunkID> _children;
};


ChunkNode::ChunkNode(ChunkSystem & system, const ChunkID &id, Chunk &chunk)
        : _system(system), _id(id), _chunk(chunk) {}

ChunkNode::ChunkNode(const ChunkNode &other)
        : _system(other._system), _id(other._id), _chunk(other._chunk) {}

bool ChunkNode::operator<(const ChunkNode &other) const {
    return _id < other._id;
}

bool ChunkNode::operator==(const ChunkNode &other) const {
    return _id == other._id;
}


class PrivateChunkSystem {
public:
    std::map<int, LODData> _lodData;
    std::map<ChunkID, std::unique_ptr<ChunkEntry>> _chunks;
};

ChunkSystem::ChunkSystem() : _internal(new PrivateChunkSystem()) {
    _internal->_lodData.emplace(0, LODData({4000, 4000, 4000}));
}

ChunkSystem::~ChunkSystem() {
    delete _internal;
}

void ChunkSystem::setLODData(int lod, const LODData & data) {
    _internal->_lodData[lod] = data;
}

LODData & ChunkSystem::getLODData(int lod) const {
    return _internal->_lodData.at(lod);
}

double ChunkSystem::getMaxDetailSize(int lod) const {
    if (lod == 0) {
        return 1e100;
    }

    auto data = getLODData(lod - 1);
    return data.getMinDetailSize();
}

double ChunkSystem::getMinDetailSize(int lod) const {
    if (lod == _maxLOD) {
        return 0;
    }

    auto data = getLODData(lod);
    return data.getMinDetailSize();
}

int ChunkSystem::getLODForDetailSize(double detailSize) const {
    int lod = 0;

    for (; lod < _maxLOD ; lod++) {
        double min = getMinDetailSize(lod);
        double max = getMaxDetailSize(lod);

        if (detailSize >= min && detailSize < max) {
            break;
        }
        else if (detailSize >= max && lod == 0) {
            break;
        }
    }

    return lod;
}

LODData & ChunkSystem::getOrCreateLODData(int lod) {
    auto lodData = _internal->_lodData.find(lod);

    if (lodData == _internal->_lodData.end()) {
        auto dimensions = _internal->_lodData[0].getChunkSize() * pow(2, -lod);
        return _internal->_lodData[lod] = LODData(dimensions);
    }
    else {
        return (*lodData).second;
    }
}

std::pair<ChunkNode, bool> ChunkSystem::getOrCreateChunkID(const vec3d& position, int lod) {
    LODData& data = _internal->_lodData[lod];
    vec3d intPos = position / data.getChunkSize();

    ChunkID id = ChunkID(
            (int) floor(intPos.x),
            (int) floor(intPos.y),
            (int) floor(intPos.z), lod);

    bool created = createChunk(id);
    return {getChunkNode(id), created};
}

std::pair<ChunkNode, bool> ChunkSystem::getOrCreateNeighbourID(const ChunkNode &chunk, const maths::vec3i &direction) {
    ChunkID id = chunk._id;
    ChunkID nid = ChunkID(id.getPosition3D() + direction, id.getLOD());

    bool created = createChunk(nid);
    printf("%p\n", getChunk(nid));
    return {getChunkNode(nid), created};
}

Chunk& ChunkSystem::getChunk(const ChunkID &id) {
    return _internal->_chunks[id]->_chunk;
}

ChunkNode ChunkSystem::getChunkNode(const ChunkID &id) {
    return ChunkNode(*this, id, getChunk(id));
}

bool ChunkSystem::createChunk(ChunkID id) {
    auto chunk = _internal->_chunks.find(id);

    if (chunk == _internal->_chunks.end()) {
        // Creation d'un nouveau chunk
        int lod = id.getLOD();
        LODData & lodData = getOrCreateLODData(lod);
        auto& chunkSize = lodData.getChunkSize();

        auto entry = std::make_unique<ChunkEntry>(id, id.getPosition3D() * chunkSize, chunkSize);
        entry->_chunk.setDetailSizeBounds(getMinDetailSize(lod), getMaxDetailSize(lod));
        _internal->_chunks[id] = std::move(entry);
        return true;
    }
    else {
        return false;
    }
}
