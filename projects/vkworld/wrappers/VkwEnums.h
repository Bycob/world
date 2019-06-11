#pragma once

namespace world {
enum class DescriptorType { STORAGE_BUFFER = 0, UNIFORM_BUFFER = 1 };

enum class MemoryType { GPU_ONLY = 0, CPU_WRITES = 1, CPU_READS = 2 };
} // namespace world