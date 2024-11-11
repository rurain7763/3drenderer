#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

void on_key_down(SDL_Keycode keycode);
void on_key_up(SDL_Keycode keycode);

void update_input();

int is_key_down(SDL_Keycode keycode);
int is_key_pressed(SDL_Keycode keycode);

#endif
