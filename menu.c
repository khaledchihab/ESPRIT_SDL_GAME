#include "menu.h"
#include "assets.h"
#include <stdio.h>
#include <string.h>

// Global variables - renamed to avoid conflicts with main.c
SDL_Surface *background_menu = NULL;
SDL_Surface *button_image = NULL;
TTF_Font *menu_font = NULL;
Mix_Chunk *hover_sound = NULL;
Button main_buttons[5];
MenuState current_menu = MAIN_MENU;

// Updated to use existing screen from main.c
int init_menu(SDL_Surface *existing_screen) {
    // We don't initialize SDL here anymore since it's already done in main.c
    // Just load assets and set up menu components
    
    // Load assets
    if (!load_menu_assets()) {
        printf("Failed to load menu assets\n");
        return 0;
    }
    
    // Initialize buttons with correct screen dimensions
    main_buttons[0] = create_button(SCREEN_WIDTH / 2, 200, "Jouer");
    main_buttons[1] = create_button(SCREEN_WIDTH / 2, 260, "Options");
    main_buttons[2] = create_button(SCREEN_WIDTH / 2, 320, "Meilleurs Scores");
    main_buttons[3] = create_button(SCREEN_WIDTH / 2, 380, "Histoire");
    main_buttons[4] = create_button(SCREEN_WIDTH / 2, 440, "Quitter");
    
    return 1;
}

int load_menu_assets() {
    // Load background
    background_menu = load_asset_image(MENU_BACKGROUND_PATH);
    if (background_menu == NULL) {
        printf("Failed to load menu background: %s\n", IMG_GetError());
        // Fall back to a colored background if image fails to load
        background_menu = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
        SDL_FillRect(background_menu, NULL, SDL_MapRGB(background_menu->format, 0, 0, 100)); // Dark blue fallback
    }
      // Load button image
    button_image = load_asset_image(MENU_BUTTON_PATH);
    if (button_image == NULL) {
        printf("Failed to load button image: %s\n", IMG_GetError());
        // Create a default button if image fails to load
        button_image = SDL_CreateRGBSurface(SDL_SWSURFACE, 200, 50, 32, 0, 0, 0, 0);
        SDL_FillRect(button_image, NULL, SDL_MapRGB(button_image->format, 100, 100, 100));
    }
    
    // Load hover sound
    hover_sound = load_asset_sound(SOUND_PATH "hover.wav");
    if (hover_sound == NULL) {
        printf("Failed to load hover sound: %s\n", Mix_GetError());
        // Non-fatal error, continue without sound
    }
    
    return 1; // Return success even if some assets fail - we have fallbacks
}

void cleanup_menu() {
    // Free menu resources only, not SDL subsystems
    if (background_menu) SDL_FreeSurface(background_menu);
    if (button_image) SDL_FreeSurface(button_image);
    if (hover_sound) Mix_FreeChunk(hover_sound);
}

Button create_button(int x, int y, const char *label) {
    Button button;
    
    // Set position - fixed for the new screen dimensions
    if (button_image) {
        button.position.x = x - button_image->w / 2; // Center horizontally
        button.position.y = y;
        button.position.w = button_image->w;
        button.position.h = button_image->h;
    } else {
        // Default dimensions if button_image failed to load
        button.position.x = x - 100;
        button.position.y = y;
        button.position.w = 200;
        button.position.h = 50;
    }
    
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
    
    // Create text with the global font
    if (TTF_GetFontStyle(menu_font) != TTF_STYLE_NORMAL) {
        TTF_SetFontStyle(menu_font, TTF_STYLE_NORMAL); // Reset font style
    }
    
    if (button->state == BUTTON_HOVER) {
        text_surface = TTF_RenderText_Solid(menu_font, button->label, hover_color);
    } else {
        text_surface = TTF_RenderText_Solid(menu_font, button->label, text_color);
    }
    
    if (text_surface) {
        // Center text on button
        offset.x = button->position.x + (button->position.w - text_surface->w) / 2;
        offset.y = button->position.y + (button->position.h - text_surface->h) / 2;
        
        // Apply text
        SDL_BlitSurface(text_surface, NULL, screen, &offset);
        
        // Free text surface
        SDL_FreeSurface(text_surface);
    }
}

void render_main_menu(SDL_Surface *screen) {
    // Clear screen
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    
    // Apply background
    SDL_BlitSurface(background_menu, NULL, screen, NULL);
    
    // Render title with our menu font
    SDL_Surface *title;
    SDL_Color title_color = {255, 255, 255};
    SDL_Rect title_pos;
    
    if (menu_font) {
        TTF_SetFontStyle(menu_font, TTF_STYLE_BOLD);
        title = TTF_RenderText_Solid(menu_font, "GAME MENU", title_color);
        if (title) {
            title_pos.x = (SCREEN_WIDTH - title->w) / 2;
            title_pos.y = 100;
            SDL_BlitSurface(title, NULL, screen, &title_pos);
            SDL_FreeSurface(title);
        }
    }
    
    // Render buttons
    for (int i = 0; i < 5; i++) {
        render_button(screen, &main_buttons[i]);
    }
    
    // Don't call SDL_Flip here - let main.c handle screen flipping
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
    if (hover_sound) {
        Mix_PlayChannel(-1, hover_sound, 0);
    }
}

// Implementations for submenu rendering functions
void render_play_menu(SDL_Surface *screen) {
    // Clear screen
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    
    // Apply background
    SDL_BlitSurface(background_menu, NULL, screen, NULL);
    
    // Render title
    SDL_Surface *title;
    SDL_Color title_color = {255, 255, 255};
    SDL_Rect title_pos;
    
    if (menu_font) {
        TTF_SetFontStyle(menu_font, TTF_STYLE_BOLD);
        title = TTF_RenderText_Solid(menu_font, "SAVE/LOAD GAME", title_color);
        if (title) {
            title_pos.x = (SCREEN_WIDTH - title->w) / 2;
            title_pos.y = 100;
            SDL_BlitSurface(title, NULL, screen, &title_pos);
            SDL_FreeSurface(title);
        }
    }
    
    // Draw info text
    SDL_Surface *info;
    SDL_Rect info_pos;
    
    if (menu_font) {
        TTF_SetFontStyle(menu_font, TTF_STYLE_NORMAL);
        info = TTF_RenderText_Solid(menu_font, "Press any key to return", title_color);
        if (info) {
            info_pos.x = (SCREEN_WIDTH - info->w) / 2;
            info_pos.y = SCREEN_HEIGHT - 100;
            SDL_BlitSurface(info, NULL, screen, &info_pos);
            SDL_FreeSurface(info);
        }
    }
    
    // Don't call SDL_Flip here
}

// Similar implementations for other menu rendering functions
void render_options_menu(SDL_Surface *screen) {
    // Clear screen
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    
    // Apply background
    SDL_BlitSurface(background_menu, NULL, screen, NULL);
    
    // Render title
    SDL_Surface *title;
    SDL_Color title_color = {255, 255, 255};
    SDL_Rect title_pos;
    
    if (menu_font) {
        TTF_SetFontStyle(menu_font, TTF_STYLE_BOLD);
        title = TTF_RenderText_Solid(menu_font, "OPTIONS", title_color);
        if (title) {
            title_pos.x = (SCREEN_WIDTH - title->w) / 2;
            title_pos.y = 100;
            SDL_BlitSurface(title, NULL, screen, &title_pos);
            SDL_FreeSurface(title);
        }
        
        // Render actual options
        // Volume control
        SDL_Surface *volume_text;
        SDL_Rect volume_pos;
        TTF_SetFontStyle(menu_font, TTF_STYLE_NORMAL);
        volume_text = TTF_RenderText_Solid(menu_font, "Volume: + / -", title_color);
        if (volume_text) {
            volume_pos.x = (SCREEN_WIDTH - volume_text->w) / 2;
            volume_pos.y = 200;
            SDL_BlitSurface(volume_text, NULL, screen, &volume_pos);
            SDL_FreeSurface(volume_text);
        }
        
        // Back button instruction
        SDL_Surface *info;
        SDL_Rect info_pos;
        info = TTF_RenderText_Solid(menu_font, "Press ESC to return", title_color);
        if (info) {
            info_pos.x = (SCREEN_WIDTH - info->w) / 2;
            info_pos.y = SCREEN_HEIGHT - 100;
            SDL_BlitSurface(info, NULL, screen, &info_pos);
            SDL_FreeSurface(info);
        }
    }
}

void render_highscore_menu(SDL_Surface *screen) {
    // Clear screen
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    
    // Apply background
    SDL_BlitSurface(background_menu, NULL, screen, NULL);
    
    // Render title
    if (menu_font) {
        SDL_Surface *title;
        SDL_Color title_color = {255, 255, 255};
        SDL_Rect title_pos;
        
        TTF_SetFontStyle(menu_font, TTF_STYLE_BOLD);
        title = TTF_RenderText_Solid(menu_font, "MEILLEURS SCORES", title_color);
        if (title) {
            title_pos.x = (SCREEN_WIDTH - title->w) / 2;
            title_pos.y = 100;
            SDL_BlitSurface(title, NULL, screen, &title_pos);
            SDL_FreeSurface(title);
        }
        
        // Add some example scores
        TTF_SetFontStyle(menu_font, TTF_STYLE_NORMAL);
        
        const char* score_entries[] = {
            "1. Player1 - 5000",
            "2. Player2 - 4500", 
            "3. Player3 - 4000",
            "4. Player4 - 3500",
            "5. Player5 - 3000"
        };
        
        for (int i = 0; i < 5; i++) {
            SDL_Surface *score;
            SDL_Rect score_pos;
            score = TTF_RenderText_Solid(menu_font, score_entries[i], title_color);
            if (score) {
                score_pos.x = (SCREEN_WIDTH - score->w) / 2;
                score_pos.y = 200 + i * 50;
                SDL_BlitSurface(score, NULL, screen, &score_pos);
                SDL_FreeSurface(score);
            }
        }
        
        // Back instruction
        SDL_Surface *info;
        SDL_Rect info_pos;
        info = TTF_RenderText_Solid(menu_font, "Press any key to return", title_color);
        if (info) {
            info_pos.x = (SCREEN_WIDTH - info->w) / 2;
            info_pos.y = SCREEN_HEIGHT - 100;
            SDL_BlitSurface(info, NULL, screen, &info_pos);
            SDL_FreeSurface(info);
        }
    }
}

void render_history_menu(SDL_Surface *screen) {
    // Clear screen
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    
    // Apply background
    SDL_BlitSurface(background_menu, NULL, screen, NULL);
    
    if (menu_font) {
        // Render title
        SDL_Surface *title;
        SDL_Color title_color = {255, 255, 255};
        SDL_Rect title_pos;
        
        TTF_SetFontStyle(menu_font, TTF_STYLE_BOLD);
        title = TTF_RenderText_Solid(menu_font, "HISTOIRE", title_color);
        if (title) {
            title_pos.x = (SCREEN_WIDTH - title->w) / 2;
            title_pos.y = 100;
            SDL_BlitSurface(title, NULL, screen, &title_pos);
            SDL_FreeSurface(title);
        }
        
        // Story text
        TTF_SetFontStyle(menu_font, TTF_STYLE_NORMAL);
        
        const char* story_lines[] = {
            "Dans un monde ravagé par le chaos...",
            "Un héros se lève pour affronter le mal.",
            "Guidez-le à travers les terres hostiles,",
            "résolvez des énigmes et combattez les ennemis",
            "pour restaurer la paix dans le royaume."
        };
        
        for (int i = 0; i < 5; i++) {
            SDL_Surface *story_line;
            SDL_Rect story_pos;
            story_line = TTF_RenderText_Solid(menu_font, story_lines[i], title_color);
            if (story_line) {
                story_pos.x = (SCREEN_WIDTH - story_line->w) / 2;
                story_pos.y = 200 + i * 50;
                SDL_BlitSurface(story_line, NULL, screen, &story_pos);
                SDL_FreeSurface(story_line);
            }
        }
        
        // Back instruction
        SDL_Surface *info;
        SDL_Rect info_pos;
        info = TTF_RenderText_Solid(menu_font, "Press any key to return", title_color);
        if (info) {
            info_pos.x = (SCREEN_WIDTH - info->w) / 2;
            info_pos.y = SCREEN_HEIGHT - 100;
            SDL_BlitSurface(info, NULL, screen, &info_pos);
            SDL_FreeSurface(info);
        }
    }
}

// Event handlers for submenus
MenuState handle_play_menu_events(SDL_Event *event) {
    if (event->type == SDL_KEYDOWN || event->type == SDL_MOUSEBUTTONDOWN) {
        return MAIN_MENU;
    }
    return PLAY_MENU;
}

MenuState handle_options_menu_events(SDL_Event *event) {
    static int volume = MIX_MAX_VOLUME / 2;
    
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_PLUS:
            case SDLK_KP_PLUS:
                volume = (volume + 10 > MIX_MAX_VOLUME) ? MIX_MAX_VOLUME : volume + 10;
                Mix_Volume(-1, volume);
                Mix_VolumeMusic(volume);
                break;
                
            case SDLK_MINUS:
            case SDLK_KP_MINUS:
                volume = (volume - 10 < 0) ? 0 : volume - 10;
                Mix_Volume(-1, volume);
                Mix_VolumeMusic(volume);
                break;
                
            case SDLK_ESCAPE:
                return MAIN_MENU;
                
            default:
                break;
        }
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