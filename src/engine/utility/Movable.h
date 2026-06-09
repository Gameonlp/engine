//
// Created by dev on 6/3/26.
//

#ifndef MYENGINE_MOVABLE_H
#define MYENGINE_MOVABLE_H
#include <utility>
template <typename T, T DefaultValue = T{}>
class Movable {
    T value;
public:
    // Implicit constructors to act like the underlying type
    Movable(const T& val) : value(val) {}
    Movable() : value(DefaultValue) {}
    // Force Move-Only Semantics
    Movable(const Movable&) = delete;
    Movable& operator=(const Movable&) = delete;
    // The Magic Boilerplate-Reducer: Clears the 'other' object on move
    Movable(Movable&& other) noexcept : value(std::exchange(other.value, DefaultValue)) {}

    Movable& operator=(Movable&& other) noexcept {
        if (this != &other) {
            value = std::exchange(other.value, DefaultValue);
        }
        return *this;
    }
    // Accessors
    operator T() const { return value; }
    T& get() { return value; }
    const T& get() const { return value; }
};
#endif //MYENGINE_MOVABLE_H