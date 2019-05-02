#include "Lightning.h"

#include <list>

#include "world/assets/ImageUtils.h"

namespace world {

Lightning::Lightning() : _rng(time(NULL)){};

void Lightning::generateLightning(Image &img, const vec2d &from) {
    std::normal_distribution<double> angleVar(0, _angleVar);
    std::uniform_real_distribution<double> subdiv;
    std::uniform_real_distribution<double> stepLength(_minStep, _maxStep);

    struct Item {
        vec2d position;
        double angle;
    };
    std::list<Item> items;
    items.emplace_back(Item{from, M_PI / 2.0});

    for (u32 i = 0; i < _stepCount; ++i) {
        std::list<Item> toAdd;

        for (auto &item : items) {
            vec2d nextPos =
                item.position +
                vec2d(cos(item.angle), sin(item.angle)) * stepLength(_rng);
            ImageUtils::drawLine(img, item.position, nextPos, 1,
                                 Color4d(1, 1, 1));

            item.position = nextPos;

            if (subdiv(_rng) < _subdivideChance &&
                items.size() + toAdd.size() < _maxSubdivisions) {
                toAdd.push_back(item);
                toAdd.back().angle += angleVar(_rng);
            }

            item.angle += angleVar(_rng);
        }

        for (auto &added : toAdd) {
            items.push_back(added);
        }
    }
}


JitterLightning::JitterLightning() : _rng(time(NULL)) {}

void JitterLightning::generateLightning(Image &img, const vec2d &from,
                                        const vec2d &to) {
    std::uniform_real_distribution<double> jitter(-_jitterMax, _jitterMax);
    std::uniform_real_distribution<double> subdivide;

    struct Segment {
        vec2d from;
        vec2d to;
        double intensity;
    };

    std::list<Segment> segments;
    segments.emplace_back(Segment{from, to, _startIntensity});

    for (int i = 0; i < _stepCount; ++i) {
        for (auto it = segments.begin(); it != segments.end(); ++it) {
            std::cout << "start: " << std::distance(segments.begin(), it) << "-"
                      << std::distance(it, segments.end()) << std::endl;
            Segment s = *it;
            vec2d dirVec = s.to - s.from;
            vec2d normVec(dirVec.y, -dirVec.x);
            vec2d newPoint = s.from + dirVec * 0.5 + normVec * jitter(_rng);
            *it = Segment{s.from, newPoint, s.intensity};
            segments.insert(it, Segment{newPoint, s.to, s.intensity});
            std::cout << "after insert #2: "
                      << std::distance(segments.begin(), it) << "-"
                      << std::distance(it, segments.end()) << std::endl;

            if (subdivide(_rng) < _subdivideChance) {
                segments.insert(it, Segment{newPoint, newPoint + dirVec * 0.35,
                                            s.intensity / 2});
                std::cout << "after insert #3: "
                          << std::distance(segments.begin(), it) << "-"
                          << std::distance(it, segments.end()) << std::endl;
            }
            std::cout << std::endl;
        }
        std::cout << "======" << std::endl;
    }

    for (Segment &segment : segments) {
        ImageUtils::drawLine(img, segment.from, segment.to, segment.intensity,
                             Color4d(1, 1, 1));
    }
}

} // namespace world