/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <cstdlib>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Root.h"
#include "State.h"
#include "Game.h"
#include "GameConfig.h"

class SDLMain {
    static Uint64 PREVIOUS, NOW;

    /* This function runs once at startup. */
    static SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
        Game *game = CreateGame();
        auto config = game->getConfig();
        auto *state = new State();
        state->root = new Root(config);
        game->onInit(state->root);
        state->root->initialize();
        if (!state->root->isValid()) {
            return SDL_APP_FAILURE;
        }
        *appstate = state;

        return SDL_APP_CONTINUE;
    }

    /* This function runs when a new event (mouse input, keypresses, etc) occurs. */
    static SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
        const auto state = static_cast<State *>(appstate);
        if (state->game->onEvent(event)) {
            return SDL_APP_CONTINUE;
        }
        if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE ||
            event->type == SDL_EVENT_QUIT) {
            return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
            }
        return SDL_APP_CONTINUE;
    }

    /* This function runs once per frame, and is the heart of the program. */
    static SDL_AppResult SDL_AppIterate(void *appstate) {
        const auto state = static_cast<State *>(appstate);
        NOW = SDL_GetTicks();
        state->root->update(NOW - PREVIOUS);
        if (!state->root->isValid()) {
            return SDL_APP_FAILURE;
        }
        PREVIOUS = NOW;

        state->root->driveDraw();
        if (!state->root->isValid()) {
            return SDL_APP_FAILURE;
        }

        return SDL_APP_CONTINUE;
    }

    /* This function runs once at shutdown. */
    static void SDL_AppQuit(void *appstate, SDL_AppResult result) {
        const auto state = static_cast<State *>(appstate);
        state->game->onShutdown();
        delete state->game;
        delete state->root;
        delete state;
    }
};