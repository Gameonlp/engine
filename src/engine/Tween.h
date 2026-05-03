//
// Created by dev on 4/19/26.
//

#ifndef MYENGINE_TWEEN_H
#define MYENGINE_TWEEN_H
#include <functional>

#include "Easing.h"
#include "GameObject.h"
#include "SDLExtensions.h"


template<typename T>
class Tween : public GameObject {
public:
    Tween(T *value, const T endValue, const float durationInSeconds, float (*ease)(float) = linear,
          const std::function<void()> &callback = {}) : GameObject({0, 0}) {
        this->value = value;
        this->endValue = endValue;
        this->startValue = *value;
        this->duration = durationInSeconds * 1000;
        this->ease = ease;
        this->elapsed = 0;
        this->callback = callback;
    }

    void update(const float dt) override {
        if (duration <= 0)
            return;
        elapsed += dt;
        if (elapsed >= duration) {
            *value = endValue;
            if (callback) {
                callback();
            }
            queueRemove();
            return;
        }
        const float ratio = std::min(elapsed / duration, 1.0f);
        *value = startValue + (endValue - startValue) * ease(ratio);
    }
private:
    T startValue, endValue;
    T *value;
    float elapsed, duration;

    float (*ease)(float);

    std::function<void()> callback;
};

#endif //ENGINE_TWEEN_H
