#ifndef WORLD_VOXELGRID_H
#define WORLD_VOXELGRID_H

#include "world/core/WorldConfig.h"

#include <memory>

#include "world/core/WorldTypes.h"
#include "world/math/Vector.h"
#include "world/math/BoundingBox.h"
#include "world/assets/Mesh.h"

namespace world {

template <typename data_t> class WORLDAPI_EXPORT VoxelGrid {
public:
    explicit VoxelGrid(u32 x, u32 y, u32 z)
            : _dims(x, y, z), _bbox({0}, {1}), _voxels(new data_t[x * y * z]) {}

    explicit VoxelGrid(const vec3u &dims)
            : _dims(dims), _bbox({0}, {1}),
              _voxels(new data_t[dims.z * dims.y * dims.z]) {}


    VoxelGrid(const vec3u &dims, data_t initVal) : VoxelGrid(dims) {
        const u32 count = this->count();
        for (u32 i = 0; i < count; ++i) {
            _voxels.get()[i] = initVal;
        }
    }

    u32 count() const { return _dims.x * _dims.y * _dims.z; }

    vec3u dims() const { return _dims; }

    const BoundingBox &bbox() const { return _bbox; }

    BoundingBox &bbox() { return _bbox; }

    data_t *values() { return _voxels.get(); }

    const data_t *values() const { return _voxels.get(); }

    data_t &at(u32 x, u32 y, u32 z) {
        return _voxels.get()[_dims.x * _dims.y * z + _dims.x * y + x];
    }

    data_t &at(const vec3u &xyz) { return at(xyz.x, xyz.y, xyz.z); }

    const data_t &at(u32 x, u32 y, u32 z) const {
        return _voxels.get()[_dims.x * _dims.y * z + _dims.x * y + x];
    }

    const data_t &at(const vec3u &xyz) const { return at(xyz.x, xyz.y, xyz.z); }

    void fill(data_t value) {
        const u32 count = this->count();
        for (u32 i = 0; i < count; ++i) {
            _voxels.get()[i] = value;
        }
    }

    VoxelGrid<data_t> copy() const {
        VoxelGrid<data_t> copy(_dims);
        const u32 count = this->count();

        for (u32 i = 0; i < count; ++i) {
            copy._voxels.get()[i] = _voxels.get()[i];
        }
        return copy;
    }

    void fromMesh(const Mesh &mesh);

    void fillMesh(Mesh &mesh) const { fillMesh(mesh, _bbox); }

    void fillMesh(Mesh &mesh, const BoundingBox &bbox) const;

protected:
    u8 getMarchingCubePolicy(const vec3u &i) const;

private:
    vec3u _dims;
    BoundingBox _bbox;
    std::shared_ptr<data_t> _voxels;
};

typedef VoxelGrid<double> VoxelField;

} // namespace world

#include "VoxelGrid.inl"

#endif // WORLD_VOXELGRID_H
