//
// Created by dev on 5/3/26.
//

#ifndef ENGINE_DIRECTION_H
#define ENGINE_DIRECTION_H
#include "utility"

enum class Direction {
    UP,
    LEFT,
    RIGHT,
    DOWN,
    NONE
};

namespace DirectionUtils {
    [[nodiscard]] constexpr Direction opposite(Direction direction) {
        switch (direction) {
            case Direction::UP:
                return Direction::DOWN;
            case Direction::LEFT:
                return Direction::RIGHT;
            case Direction::RIGHT:
                return Direction::LEFT;
            case Direction::DOWN:
                return Direction::UP;
            case Direction::NONE:
                return Direction::NONE;
        }
        std::unreachable();
    }
}
#endif //ENGINE_DIRECTION_H
