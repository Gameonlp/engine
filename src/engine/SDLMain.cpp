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

Uint64 PREVIOUS = SDL_GetTicks(), NOW;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    Game *game = CreateGame();
    auto config = game->getConfig();
    auto *state = new State();
    SDL_Window *window;
    SDL_Renderer *renderer;
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Hello World", config.SCREEN_WIDTH, config.SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN,
                                     &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, config.SCREEN_WIDTH, config.SCREEN_HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);
    state->root = new Root(renderer, window);
    game->onInit(state->root);
    *appstate = state;


    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
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
SDL_AppResult SDL_AppIterate(void *appstate) {
    const auto state = static_cast<State *>(appstate);
    NOW = SDL_GetTicks();
    state->root->update(NOW - PREVIOUS);
    PREVIOUS = NOW;


    /* Draw the message */
    state->root->draw();

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    const auto state = static_cast<State *>(appstate);
    state->game->onShutdown();
    delete state->root;
    delete state;
}
