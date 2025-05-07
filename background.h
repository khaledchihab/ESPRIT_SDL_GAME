#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <time.h>

/**
 * Structure for background management
 */
typedef struct
{
    SDL_Surface* img;       /* The background image */
    SDL_Rect camera_pos;    /* Camera position for scrolling */
    SDL_Rect pos_screen;    /* Position on screen */
    int type;               /* Type of background: 0-fixed, 1-mobile, 2-destructible */
} Background;

/**
 * Structure for game time management
 */
typedef struct
{
    time_t startTime;       /* Time when the game started */
    TTF_Font* font;         /* Font for displaying time */
    SDL_Surface* timeText;  /* Surface to render time text */
    SDL_Rect timePos;       /* Position for time display */
} GameTime;

/**
 * Initialize a background
 * @param bg Pointer to background structure
 * @param filename Image file path
 * @param type Background type: 0-fixed, 1-mobile, 2-destructible
 */
void initBackground(Background* bg, char* filename, int type);

/**
 * Initialize game time
 * @param gameTime Pointer to the GameTime structure
 * @param fontPath Path to the TTF font file
 * @param fontSize Size of the font
 */
void initGameTime(GameTime* gameTime, char* fontPath, int fontSize);

/**
 * Display the background
 * @param bg Background to display
 * @param screen Screen surface to blit on
 */
void displayBackground(Background bg, SDL_Surface* screen);

/**
 * Scroll the background
 * @param bg Pointer to the background
 * @param direction Direction of scrolling (0-up, 1-down, 2-left, 3-right)
 * @param speed Scrolling speed
 */
void scrollBackground(Background* bg, int direction, int speed);

/**
 * Update and display the game time
 * @param gameTime Pointer to the GameTime structure
 * @param screen Screen surface to blit on
 */
void updateGameTime(GameTime* gameTime, SDL_Surface* screen);

/**
 * Free resources used by the background
 * @param bg Pointer to the background
 */
void freeBackground(Background* bg);

/**
 * Free resources used by the game time
 * @param gameTime Pointer to the GameTime structure
 */
void freeGameTime(GameTime* gameTime);

/**
 * Split screen into multiple views (for multiplayer or minimap)
 * @param screen Main screen surface
 * @param surfaces Array of surfaces to display
 * @param positions Array of positions for each surface
 * @param count Number of partitions
 */
void partitionScreen(SDL_Surface* screen, SDL_Surface** surfaces, SDL_Rect* positions, int count);

#endif /* BACKGROUND_H */