#pragma once

namespace world {
// TODO Try to remove use of this enum
enum class DescriptorType {
    STORAGE_BUFFER = 0,
    UNIFORM_BUFFER = 1,
    VERTEX_BUFFER = 2,
    INDEX_BUFFER = 3,
    TRANSFER_SRC = 4,
    TRANSFER_DST = 5,
    IMAGE = 4,
};

enum class MemoryUsage { GPU_ONLY = 0, CPU_WRITES = 1, CPU_READS = 2 };
} // namespace world