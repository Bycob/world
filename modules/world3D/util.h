//
// Created by louis on 22/04/17.
//

#ifndef WORLD_UTIL_H_H
#define WORLD_UTIL_H_H

inline void sleep(float ms) {
    std::this_thread::sleep_for(std::chrono::microseconds((int) (ms * 1000)));
}

#endif //WORLD_UTIL_H_H
