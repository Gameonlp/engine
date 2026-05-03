//
// Created by dev on 4/26/26.
//

#include "Timer.h"

bool Timer::isRunning() const {
    return timeLeft != 0;
}

void Timer::start(float time) {
    if (time != 0) {
        this->time = time;
    }
    timeLeft = this->time;
}

void Timer::stop() {
    timeLeft = 0;
}

void Timer::update(const float dt) {
    if (timeLeft == 0) {
        return;
    }
    timeLeft -= dt;
    if (timeLeft <= 0) {
        stop();
    }
}
