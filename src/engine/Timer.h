//
// Created by dev on 4/26/26.
//

#ifndef SNAKE_TIMER_H
#define SNAKE_TIMER_H
#include "GameObject.h"


class Timer : public GameObject {
public:
    Timer(float time) : GameObject({0, 0}), time(time) {};
    void start(float time = 0);
    void stop();
    [[nodiscard]] bool isRunning() const;
    void update(float dt) override;
private:
    float time;
    float timeLeft = 0;
};


#endif //SNAKE_TIMER_H