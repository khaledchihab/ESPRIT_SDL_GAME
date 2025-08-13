#ifndef MENU_H
#define MENU_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

// Screen dimensions - updated to match main.c
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720 
#define SCREEN_BPP 32

// Menu states
typedef enum {
    MAIN_MENU,
    PLAY_MENU,
    OPTIONS_MENU,
    HIGHSCORE_MENU,
    HISTORY_MENU,
    START_GAME,
    QUIT
} MenuState;

// Button states
typedef enum {
    BUTTON_NORMAL,
    BUTTON_HOVER,
    BUTTON_CLICK
} ButtonState;

// Button structure
typedef struct {
    SDL_Surface *image;
    SDL_Rect position;
    ButtonState state;
    char label[20];
} Button;

// Function prototypes
// Initialization
int init_menu(SDL_Surface *existing_screen); // Updated to use existing screen
int load_menu_assets();
void cleanup_menu();

// Button management
Button create_button(int x, int y, const char *label);
int button_hover(Button *button, int x, int y);
void render_button(SDL_Surface *screen, Button *button);

// Menu functions
void render_main_menu(SDL_Surface *screen);
MenuState handle_main_menu_events(SDL_Event *event);

// Add additional function prototypes for submenu rendering
void render_play_menu(SDL_Surface *screen);
void render_options_menu(SDL_Surface *screen);
void render_highscore_menu(SDL_Surface *screen);
void render_history_menu(SDL_Surface *screen);

// Add submenu event handlers
MenuState handle_play_menu_events(SDL_Event *event);
MenuState handle_options_menu_events(SDL_Event *event);
MenuState handle_highscore_menu_events(SDL_Event *event);
MenuState handle_history_menu_events(SDL_Event *event);

// Sound functions
void play_hover_sound();

// Global variables declarations (to be defined in source file)
extern SDL_Surface *background_menu;
extern SDL_Surface *button_image;
extern TTF_Font *menu_font;
extern Mix_Chunk *hover_sound;
extern Button main_buttons[5];
extern MenuState current_menu;

#endif