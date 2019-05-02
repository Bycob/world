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
} // namespace world