#include "input.h"

static unsigned long key_state = 0;
static unsigned long key_down_state = 0;

unsigned long sdl_keycode_to_bitmask(SDL_Keycode keycode) {
	switch (keycode) {
		case SDLK_ESCAPE: return 1l << 0;
		case SDLK_DOWN: return 1l << 1;
		case SDLK_UP: return 1l << 2;
		case SDLK_LEFT: return 1l << 3;
		case SDLK_RIGHT: return 1l << 4;
		case SDLK_a: return 1l << 5;
		case SDLK_d: return 1l << 6;
		case SDLK_w: return 1l << 7;
		case SDLK_s: return 1l << 8;
		case SDLK_n: return 1l << 9;
	}
	return 0;
}

void on_key_down(SDL_Keycode keycode) {
	unsigned long key_code = sdl_keycode_to_bitmask(keycode);
	key_state |= key_code;
	key_down_state |= key_code;
}

void on_key_up(SDL_Keycode keycode) {
	unsigned long key_code = sdl_keycode_to_bitmask(keycode);
	key_state &= ~key_code;
}

void update_input() {
	key_down_state = 0;
}

int is_key_down(SDL_Keycode keycode) {
	unsigned long key_code = sdl_keycode_to_bitmask(keycode);
	return key_down_state & key_code;
}

int is_key_pressed(SDL_Keycode keycode) {
	unsigned long key_code = sdl_keycode_to_bitmask(keycode);
	return key_state & key_code;
}




