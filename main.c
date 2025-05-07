#include "menu.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    // Initialize SDL and subsystems
    if (!init_SDL()) {
        printf("Failed to initialize SDL!\n");
        return 1;
    }
    
    // Load assets
    if (!load_assets()) {
        printf("Failed to load assets!\n");
        cleanup();
        return 1;
    }
    
    // Main game loop
    SDL_Event event;
    int quit = 0;
    
    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
            
            // Handle menu-specific events
            switch (current_menu) {
                case MAIN_MENU:
                    current_menu = handle_main_menu_events(&event);
                    if (current_menu == QUIT) {
                        quit = 1;
                    }
                    break;
                    
                case PLAY_MENU:
                    current_menu = handle_play_menu_events(&event);
                    break;
                    
                case OPTIONS_MENU:
                    current_menu = handle_options_menu_events(&event);
                    break;
                    
                case HIGHSCORE_MENU:
                    current_menu = handle_highscore_menu_events(&event);
                    break;
                    
                case HISTORY_MENU:
                    current_menu = handle_history_menu_events(&event);
                    break;
                    
                default:
                    break;
            }
        }
        
        // Render current menu
        switch (current_menu) {
            case MAIN_MENU:
                render_main_menu(screen);
                break;
                
            case PLAY_MENU:
                render_play_menu(screen);
                break;
                
            case OPTIONS_MENU:
                render_options_menu(screen);
                break;
                
            case HIGHSCORE_MENU:
                render_highscore_menu(screen);
                break;
                
            case HISTORY_MENU:
                render_history_menu(screen);
                break;
                
            default:
                break;
        }
        
        // Cap the frame rate
        SDL_Delay(10);
    }
    
    // Cleanup before exit
    cleanup();
    
    return 0;
}