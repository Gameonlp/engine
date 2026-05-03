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

#include "Board.h"
#include "Constants.h"
#include "Snake.h"
#include "engine/Root.h"
#include "engine/State.h"
#include "engine/Utils.h"

Uint64 PREVIOUS = SDL_GetTicks(), NOW;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    auto *state = new State();
    SDL_Window *window;
    SDL_Renderer *renderer;
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Hello World", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    state->root = new Root(renderer, window);
    auto board = std::make_unique<Board>();
    board->setUpGame();
    state->root->queueAddChild(std::move(board));
    *appstate = state;


    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    const auto state = static_cast<State *>(appstate);
    NOW = SDL_GetTicks();
    state->root->update(NOW - PREVIOUS);
    PREVIOUS = NOW;


    /* Draw the message */
    state->root->draw();

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    const auto state = static_cast<State *>(appstate);
    delete state->root;
    delete state;
}