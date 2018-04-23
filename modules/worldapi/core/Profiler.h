#ifndef WORLD_PROFILER_H
#define WORLD_PROFILER_H

#include "core/WorldConfig.h"

#include <iostream>
#include <string>
#include <chrono>
#include <list>

namespace world {
class Profiler {
public:
    using time_point = std::chrono::steady_clock::time_point;

    struct Section {
        time_point start;
        time_point end;
        std::string name;
        bool terminated = false;
    };

    Profiler() {}

    void endSection() {
        auto now = std::chrono::steady_clock::now();

        if (!_sections.empty() && !_sections.back().terminated) {
            _sections.back().end = now;
            _sections.back().terminated = true;
        }
    }

    void endStartSection(const std::string &name) {
        endSection();

        _sections.emplace_back();
        auto &currentSection = _sections.back();
        currentSection.name = name;
        currentSection.start = std::chrono::steady_clock::now();
    }

    void dump() {
        std::cout << "Profiler results :" << std::endl;

        for (auto &section : _sections) {
            if (!section.terminated)
                continue;

            std::cout << " - Section \"" << section.name << "\" lasted "
                      << std::chrono::duration_cast<std::chrono::microseconds>(
                             section.end - section.start)
                             .count()
                      << " us" << std::endl;
        }
    }

private:
    std::list<Section> _sections;
};
} // namespace world

#endif // WORLD_PROFILER_H
