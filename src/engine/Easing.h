//
// Created by dev on 4/19/26.
//

#ifndef MYENGINE_EASING_H
#define MYENGINE_EASING_H

static float linear(float t) {
    return t;
}

static float easeIn(float t) {
    return t * t * t;
}

static float easeOut(float t) {
    return 1.0f - easeIn(1.0f - t);
}

template<int s_in_permille = 1701>
static float easeInBack(float t) {
    const float s = s_in_permille / 1000.0f;
    return t * t * ((s + 1.0f) * t - s);
}

template<int s_in_permille = 1701>
static float easeOutBack(float t) {
    return 1.0f - easeInBack<s_in_permille>(1.0f - t);
}

#endif //ENGINE_EASING_H
