//
// Created by dev on 5/3/26.
//

#ifndef MYENGINE_GAME_H
#define MYENGINE_GAME_H

struct GameConfig;

class Game {
public:
    virtual ~Game() = default;

    virtual GameConfig getConfig();
    virtual void onInit(Root *root) = 0;
    virtual bool onEvent(const SDL_Event *event) = 0;
    virtual void onShutdown() = 0;
};

Game *CreateGame();

#endif //MYENGINE_GAME_H