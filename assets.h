#ifndef ASSETS_H
#define ASSETS_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

// Screen dimensions
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_BPP 32

// Base asset paths
#define ASSET_PATH "assets/"
#define FONT_PATH "assets/fonts/"
#define MUSIC_PATH "assets/music/"
#define SOUND_PATH "assets/sounds/"
#define TEXTURE_PATH "assets/textures/"

// Player assets
#define PLAYER_SPRITE_PATH TEXTURE_PATH "player_sprite.png"

// Menu assets
#define MENU_BACKGROUND_PATH TEXTURE_PATH "menu_background.png"
#define MENU_BUTTON_PATH TEXTURE_PATH "menu_button.png"
#define MENU_FONT_PATH FONT_PATH "font.ttf"

// Background assets
#define BACKGROUND_PATH TEXTURE_PATH "background1.png"

// Enemy assets
#define ENEMY_SPRITE_PATH TEXTURE_PATH "enemy_sprite.png"

// Enigme assets
#define ENIGME1_PATH "assets/enigme1/"
#define ENIGME2_PATH "assets/enigme2/"

// Sound assets
#define BACKGROUND_MUSIC MUSIC_PATH "drumloop.wav"
#define EFFECT_SOUND SOUND_PATH "effect.wav"

// Helper functions
SDL_Surface* load_asset_image(const char* path);
TTF_Font* load_asset_font(const char* path, int size);
Mix_Music* load_asset_music(const char* path);
Mix_Chunk* load_asset_sound(const char* path);

// Asset verification
int verify_assets(); // Returns 1 if all critical assets exist, 0 otherwise

#endif // ASSETS_H