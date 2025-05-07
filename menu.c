#include "menu.h"
#include <stdio.h>
#include <string.h>

// Global variables
SDL_Surface *screen = NULL;
SDL_Surface *background = NULL;
SDL_Surface *button_image = NULL;
TTF_Font *font = NULL;
Mix_Music *music = NULL;
Mix_Chunk *hover_sound = NULL;
Button main_buttons[5];
MenuState current_menu = MAIN_MENU;

int init_SDL() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 0;
    }
    
    // Set up screen
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
    if (screen == NULL) {
        printf("Screen setup failed: %s\n", SDL_GetError());
        return 0;
    }
    
    // Initialize SDL_mixer
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        printf("SDL_mixer initialization failed: %s\n", Mix_GetError());
        return 0;
    }
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf initialization failed: %s\n", TTF_GetError());
        return 0;
    }
    
    // Set window caption
    SDL_WM_SetCaption("Game Menu", NULL);
    
    return 1;
}

int load_assets() {
    // Load background
    background = IMG_Load("assets/background1.png");
    if (background == NULL) {
        printf("Failed to load background: %s\n", IMG_GetError());
        return 0;
    }
    
    // Load button image
    button_image = IMG_Load("assets/button.png");
    if (button_image == NULL) {
        printf("Failed to load button image: %s\n", IMG_GetError());
        return 0;
    }
    
    // Load font
    font = TTF_OpenFont("assets/font.ttf", 28);
    if (font == NULL) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 0;
    }
    
    // Load music
    music = Mix_LoadMUS("assets/drumbeat.wav");
    if (music == NULL) {
        printf("Failed to load music: %s\n", Mix_GetError());
        return 0;
    }
    
    // Load hover sound
    hover_sound = Mix_LoadWAV("assets/hover.wav");
    if (hover_sound == NULL) {
        printf("Failed to load hover sound: %s\n", Mix_GetError());
        return 0;
    }
    
    // Initialize buttons
    main_buttons[0] = create_button(SCREEN_WIDTH / 2, 200, "Jouer");
    main_buttons[1] = create_button(SCREEN_WIDTH / 2, 260, "Options");
    main_buttons[2] = create_button(SCREEN_WIDTH / 2, 320, "Meilleurs Scores");
    main_buttons[3] = create_button(SCREEN_WIDTH / 2, 380, "Histoire");
    main_buttons[4] = create_button(SCREEN_WIDTH / 2, 440, "Quitter");
    
    // Play background music
    Mix_PlayMusic(music, -1);
    
    return 1;
}

void cleanup() {
    // Free surfaces
    if (background) SDL_FreeSurface(background);
    if (button_image) SDL_FreeSurface(button_image);
    
    // Free font
    if (font) TTF_CloseFont(font);
    
    // Free audio
    if (music) Mix_FreeMusic(music);
    if (hover_sound) Mix_FreeChunk(hover_sound);
    
    // Quit subsystems
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
}

Button create_button(int x, int y, const char *label) {
    Button button;
    
    // Set position
    button.position.x = x - button_image->w / 2; // Center horizontally
    button.position.y = y;
    button.position.w = button_image->w;
    button.position.h = button_image->h;
    
    // Set label and initial state
    strncpy(button.label, label, 19);
    button.label[19] = '\0';
    button.state = BUTTON_NORMAL;
    button.image = NULL;
    
    return button;
}

int button_hover(Button *button, int x, int y) {
    // Check if mouse is over button
    if (x >= button->position.x && x <= button->position.x + button->position.w &&
        y >= button->position.y && y <= button->position.y + button->position.h) {
        
        // If button state wasn't hover before, play sound
        if (button->state != BUTTON_HOVER) {
            button->state = BUTTON_HOVER;
            play_hover_sound();
        }
        return 1;
    } else {
        button->state = BUTTON_NORMAL;
        return 0;
    }
}

void render_button(SDL_Surface *screen, Button *button) {
    SDL_Surface *text_surface;
    SDL_Color text_color = {255, 255, 255}; // White
    SDL_Color hover_color = {255, 255, 0};  // Yellow
    SDL_Rect offset;
    
    // Apply button image
    SDL_BlitSurface(button_image, NULL, screen, &button->position);
    
    // Create text
    if (button->state == BUTTON_HOVER) {
        text_surface = TTF_RenderText_Solid(font, button->label, hover_color);
    } else {
        text_surface = TTF_RenderText_Solid(font, button->label, text_color);
    }
    
    // Center text on button
    offset.x = button->position.x + (button->position.w - text_surface->w) / 2;
    offset.y = button->position.y + (button->position.h - text_surface->h) / 2;
    
    // Apply text
    SDL_BlitSurface(text_surface, NULL, screen, &offset);
    
    // Free text surface
    SDL_FreeSurface(text_surface);
}

void render_main_menu(SDL_Surface *screen) {
    // Clear screen
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    
    // Apply background
    SDL_BlitSurface(background, NULL, screen, NULL);
    
    // Render title
    SDL_Surface *title;
    SDL_Color title_color = {255, 255, 255};
    SDL_Rect title_pos;
    
    title = TTF_RenderText_Solid(font, "GAME MENU", title_color);
    title_pos.x = (SCREEN_WIDTH - title->w) / 2;
    title_pos.y = 100;
    SDL_BlitSurface(title, NULL, screen, &title_pos);
    SDL_FreeSurface(title);
    
    // Render buttons
    for (int i = 0; i < 5; i++) {
        render_button(screen, &main_buttons[i]);
    }
    
    // Update screen
    SDL_Flip(screen);
}

MenuState handle_main_menu_events(SDL_Event *event) {
    int x, y;
    static int last_hover = -1;
    
    // Get mouse position
    SDL_GetMouseState(&x, &y);
    
    // Check for hover on buttons
    for (int i = 0; i < 5; i++) {
        if (button_hover(&main_buttons[i], x, y)) {
            if (last_hover != i) {
                last_hover = i;
            }
        }
    }
    
    // Handle events
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        // Check if a button was clicked
        for (int i = 0; i < 5; i++) {
            if (button_hover(&main_buttons[i], x, y)) {
                main_buttons[i].state = BUTTON_CLICK;
                switch (i) {
                    case 0: return PLAY_MENU;
                    case 1: return OPTIONS_MENU;
                    case 2: return HIGHSCORE_MENU;
                    case 3: return HISTORY_MENU;
                    case 4: return QUIT;
                }
            }
        }
    }
    else if (event->type == SDL_KEYDOWN) {
        // Handle keyboard shortcuts
        switch (event->key.keysym.sym) {
            case SDLK_j: return PLAY_MENU;
            case SDLK_o: return OPTIONS_MENU;
            case SDLK_m: return HIGHSCORE_MENU;
            case SDLK_ESCAPE: return QUIT;
            default: break;
        }
    }
    
    return MAIN_MENU;
}

void play_hover_sound() {
    Mix_PlayChannel(-1, hover_sound, 0);
}

// Add implementations for submenu rendering functions
void render_play_menu(SDL_Surface *screen) {
    // Clear screen
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    
    // Apply a dimmed version of the background for consistency
    SDL_BlitSurface(background, NULL, screen, NULL);
    
    // Render title
    SDL_Surface *title;
    SDL_Color title_color = {255, 255, 255};
    SDL_Rect title_pos;
    
    title = TTF_RenderText_Solid(font, "SAVE/LOAD GAME", title_color);
    title_pos.x = (SCREEN_WIDTH - title->w) / 2;
    title_pos.y = 100;
    SDL_BlitSurface(title, NULL, screen, &title_pos);
    SDL_FreeSurface(title);
    
    // Draw info text
    SDL_Surface *info;
    SDL_Rect info_pos;
    
    info = TTF_RenderText_Solid(font, "Press any key to return", title_color);
    info_pos.x = (SCREEN_WIDTH - info->w) / 2;
    info_pos.y = 500;
    SDL_BlitSurface(info, NULL, screen, &info_pos);
    SDL_FreeSurface(info);
    
    // Update screen
    SDL_Flip(screen);
}

void render_options_menu(SDL_Surface *screen) {
    // Clear screen
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    
    // Apply background
    SDL_BlitSurface(background, NULL, screen, NULL);
    
    // Render title
    SDL_Surface *title;
    SDL_Color title_color = {255, 255, 255};
    SDL_Rect title_pos;
    
    title = TTF_RenderText_Solid(font, "OPTIONS", title_color);
    title_pos.x = (SCREEN_WIDTH - title->w) / 2;
    title_pos.y = 100;
    SDL_BlitSurface(title, NULL, screen, &title_pos);
    SDL_FreeSurface(title);
    
    // Draw info text
    SDL_Surface *info;
    SDL_Rect info_pos;
    
    info = TTF_RenderText_Solid(font, "Press any key to return", title_color);
    info_pos.x = (SCREEN_WIDTH - info->w) / 2;
    info_pos.y = 500;
    SDL_BlitSurface(info, NULL, screen, &info_pos);
    SDL_FreeSurface(info);
    
    // Update screen
    SDL_Flip(screen);
}

void render_highscore_menu(SDL_Surface *screen) {
    // Clear screen
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    
    // Apply background
    SDL_BlitSurface(background, NULL, screen, NULL);
    
    // Render title
    SDL_Surface *title;
    SDL_Color title_color = {255, 255, 255};
    SDL_Rect title_pos;
    
    title = TTF_RenderText_Solid(font, "MEILLEURS SCORES", title_color);
    title_pos.x = (SCREEN_WIDTH - title->w) / 2;
    title_pos.y = 100;
    SDL_BlitSurface(title, NULL, screen, &title_pos);
    SDL_FreeSurface(title);
    
    // Draw info text
    SDL_Surface *info;
    SDL_Rect info_pos;
    
    info = TTF_RenderText_Solid(font, "Press any key to return", title_color);
    info_pos.x = (SCREEN_WIDTH - info->w) / 2;
    info_pos.y = 500;
    SDL_BlitSurface(info, NULL, screen, &info_pos);
    SDL_FreeSurface(info);
    
    // Update screen
    SDL_Flip(screen);
}

void render_history_menu(SDL_Surface *screen) {
    // Clear screen
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    
    // Apply background
    SDL_BlitSurface(background, NULL, screen, NULL);
    
    // Render title
    SDL_Surface *title;
    SDL_Color title_color = {255, 255, 255};
    SDL_Rect title_pos;
    
    title = TTF_RenderText_Solid(font, "HISTOIRE", title_color);
    title_pos.x = (SCREEN_WIDTH - title->w) / 2;
    title_pos.y = 100;
    SDL_BlitSurface(title, NULL, screen, &title_pos);
    SDL_FreeSurface(title);
    
    // Draw info text
    SDL_Surface *info;
    SDL_Rect info_pos;
    
    info = TTF_RenderText_Solid(font, "Press any key to return", title_color);
    info_pos.x = (SCREEN_WIDTH - info->w) / 2;
    info_pos.y = 500;
    SDL_BlitSurface(info, NULL, screen, &info_pos);
    SDL_FreeSurface(info);
    
    // Update screen
    SDL_Flip(screen);
}

// Add event handlers for submenus
MenuState handle_play_menu_events(SDL_Event *event) {
    if (event->type == SDL_KEYDOWN || event->type == SDL_MOUSEBUTTONDOWN) {
        return MAIN_MENU;
    }
    return PLAY_MENU;
}

MenuState handle_options_menu_events(SDL_Event *event) {
    if (event->type == SDL_KEYDOWN || event->type == SDL_MOUSEBUTTONDOWN) {
        return MAIN_MENU;
    }
    return OPTIONS_MENU;
}

MenuState handle_highscore_menu_events(SDL_Event *event) {
    if (event->type == SDL_KEYDOWN || event->type == SDL_MOUSEBUTTONDOWN) {
        return MAIN_MENU;
    }
    return HIGHSCORE_MENU;
}

MenuState handle_history_menu_events(SDL_Event *event) {
    if (event->type == SDL_KEYDOWN || event->type == SDL_MOUSEBUTTONDOWN) {
        return MAIN_MENU;
    }
    return HISTORY_MENU;
}