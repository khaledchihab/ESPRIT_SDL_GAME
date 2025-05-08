#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

// Include correct header files
#include "assets.h"
#include "menu.h"
#include "joueur.h"
#include "enigme1.h"
#include "enigme2.h"
#include "ennemie.h"
#include "minimap.h"
#include "background.h"

// Game states
typedef enum {
    STATE_MAIN_GAME,
    STATE_MENU,
    STATE_ENIGME1,
    STATE_ENIGME2,
    STATE_GAMEOVER
} GameState;

// Function prototypes
int init_SDL();
void cleanup_SDL();
SDL_Surface* load_image(const char* filename);
void display_stats(Joueur joueur, SDL_Surface *screen, TTF_Font *font);

int main(int argc, char *argv[]) {
    // Core SDL variables
    SDL_Surface *screen = NULL;
    SDL_Surface *background = NULL;
    SDL_Rect bgPos = {0, 0};

    TTF_Font *font = NULL;
    Mix_Music *music = NULL;
      // Initialize all SDL subsystems
    if (!init_SDL()) {
        printf("Failed to initialize SDL systems\n");
        return 1;
    }
    
    // Verify critical assets
    verify_assets(); // Continue even if some assets are missing - we have fallbacks

    // Setup screen
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (screen == NULL) {
        printf("Erreur lors de la création de la fenêtre: %s\n", SDL_GetError());
        cleanup_SDL();
        return 1;
    }
    SDL_WM_SetCaption("Mon Jeu", NULL);    // Load main game font
    font = load_asset_font(MENU_FONT_PATH, 24);
    if (font == NULL) {
        printf("Erreur lors du chargement de la police: %s\n", TTF_GetError());
        cleanup_SDL();
        return 1;
    }    // Load and play background music
    music = load_asset_music(BACKGROUND_MUSIC);
    if (music == NULL) {
        printf("Erreur lors du chargement de la musique: %s\n", Mix_GetError());
        // Continue without music
    } else {
        Mix_PlayMusic(music, -1); // Play in loop
    }    // Load background image
    background = load_asset_image(BACKGROUND_PATH);
    if (!background) {
        printf("Erreur de chargement du background: %s\n", IMG_GetError());
        cleanup_SDL();
        return 1;
    }

    // Initialize menu system with our screen
    if (!init_menu(screen)) {
        printf("Failed to initialize menu system\n");
        cleanup_SDL();
        return 1;
    }

    // Set the menu font for proper rendering
    menu_font = font;

    // Init game elements    // Player initialization
    Joueur joueur;
    initialiser_joueur(&joueur, "Player1", PLAYER_SPRITE_PATH);

    // Enemy initialization
    Enemy enemy;
    initEnemy(&enemy, LEVEL1, 800, 400);

    // Minimap initialization
    minimap mini;
    initmap(&mini);

    // Enigme initialization
    enigme en;
    SDL_Color textColor = {255, 255, 255};
    init_enigme(&en, textColor);

    // Create collision mask (usually loaded from file but creating a blank one for now)
    SDL_Surface *collision_mask = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
    SDL_FillRect(collision_mask, NULL, SDL_MapRGB(collision_mask->format, 0, 0, 0)); // Black = no collision

    // Variables
    SDL_Event event;
    int continuer = 1;
    
    // Game state management
    GameState gameState = STATE_MENU; // Start with menu instead of main game
    MenuState menuState = MAIN_MENU;
    
    // Enigme triggers
    int enigme1_trigger_x = 500;
    int enigme2_trigger_x = 1000;
    int trigger_range = 50;

    // Main game loop
    while (continuer) {
        // Process all pending events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                continuer = 0;
            }
            
            // Handle input based on current game state
            if (gameState == STATE_MENU) {
                // Process menu events
                switch (menuState) {
                    case MAIN_MENU:
                        menuState = handle_main_menu_events(&event);
                        break;
                    case PLAY_MENU:
                        menuState = handle_play_menu_events(&event);
                        break;
                    case OPTIONS_MENU:
                        menuState = handle_options_menu_events(&event);
                        break;
                    case HIGHSCORE_MENU:
                        menuState = handle_highscore_menu_events(&event);
                        break;
                    case HISTORY_MENU:
                        menuState = handle_history_menu_events(&event);
                        break;
                    case QUIT:
                        continuer = 0;
                        break;
                }
                
                // Check if we should switch from menu to game
                if (menuState == PLAY_MENU) {
                    gameState = STATE_MAIN_GAME;
                }
            }
            else if (gameState == STATE_MAIN_GAME) {
                // Game specific input handling
                if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            gameState = STATE_MENU; // Return to menu
                            menuState = MAIN_MENU;
                            break;
                        case SDLK_e:
                            // Manual enigme trigger for testing
                            gameState = STATE_ENIGME2;
                            break;
                    }
                }
            }
        }

        // Game state machine
        switch (gameState) {
            case STATE_MENU:
                // Render current menu
                switch (menuState) {
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
                break;
                
            case STATE_MAIN_GAME:
                // Get keyboard state for continuous movement
                const Uint8 *keystate = SDL_GetKeyState(NULL);
                
                // Update player based on keyboard input
                if (keystate[SDLK_RIGHT]) {
                    deplacer_joueur(&joueur, 5, 0, collision_mask);
                }
                if (keystate[SDLK_LEFT]) {
                    deplacer_joueur(&joueur, -5, 0, collision_mask);
                }
                if (keystate[SDLK_SPACE]) {
                    sauter_joueur(&joueur);
                }
                
                // Apply gravity/update jump state
                deplacer_joueur(&joueur, 0, 5, collision_mask); // Simplified gravity
                
                // Update enemy with AI targeting player
                moveEnemyAI(&enemy, joueur.position, collision_mask);
                
                // Update minimap based on player position
                MAJMinimap(joueur.position, &mini, joueur.camera, 5);
                
                // Animate characters
                animer_joueur(&joueur);
                animateEnemy(&enemy);
                
                // Render game elements
                SDL_BlitSurface(background, NULL, screen, &bgPos);
                afficher_joueur(joueur, screen);
                displayEnemy(screen, &enemy);
                afficherminimap(mini, screen);
                
                // Display player stats
                display_stats(joueur, screen, font);
                
                // Check enigme triggers
                if (abs(joueur.position.x - enigme1_trigger_x) < trigger_range) {
                    gameState = STATE_ENIGME1;
                }
                if (abs(joueur.position.x - enigme2_trigger_x) < trigger_range) {
                    gameState = STATE_ENIGME2;
                }
                
                // Check collision between player and enemy
                int collision_result = checkCollisionWithPlayer(&enemy, joueur.position);
                if (collision_result) {
                    update_lives(&joueur, -1);
                    
                    // Push player away from enemy on collision
                    if (joueur.position.x < enemy.position.x) {
                        deplacer_joueur(&joueur, -30, -10, collision_mask);
                    } else {
                        deplacer_joueur(&joueur, 30, -10, collision_mask);
                    }
                }
                
                // Check game over condition
                if (joueur.lives <= 0) {
                    gameState = STATE_GAMEOVER;
                }
                break;
                
            case STATE_ENIGME1:
                // Launch enigme1 code
                if (quiz1(&en, screen)) {
                    // If solved correctly, give reward
                    update_score(&joueur, 100);
                }
                gameState = STATE_MAIN_GAME;
                break;
                  case STATE_ENIGME2:
                // Call enigme2 function - play_enigme2 returns 1 for success, 0 for failure
                if (play_enigme2(screen)) {
                    // If solved correctly, give reward
                    update_score(&joueur, 200);
                }
                gameState = STATE_MAIN_GAME;
                break;
                
            case STATE_GAMEOVER:
                // Display game over text
                SDL_Surface *game_over;
                SDL_Rect game_over_pos;
                SDL_Color white = {255, 255, 255};
                
                // Clear screen
                SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
                
                game_over = TTF_RenderText_Solid(font, "GAME OVER", white);
                game_over_pos.x = (SCREEN_WIDTH - game_over->w) / 2;
                game_over_pos.y = (SCREEN_HEIGHT - game_over->h) / 2;
                SDL_BlitSurface(game_over, NULL, screen, &game_over_pos);
                SDL_FreeSurface(game_over);
                
                // Display score
                char score_text[50];
                sprintf(score_text, "Final Score: %d", joueur.score);
                SDL_Surface *score_surf = TTF_RenderText_Solid(font, score_text, white);
                SDL_Rect score_pos = {
                    (SCREEN_WIDTH - score_surf->w) / 2,
                    game_over_pos.y + game_over->h + 20
                };
                SDL_BlitSurface(score_surf, NULL, screen, &score_pos);
                SDL_FreeSurface(score_surf);
                
                // Display instruction to return to menu
                SDL_Surface *inst = TTF_RenderText_Solid(font, "Press ENTER to return to menu", white);
                SDL_Rect inst_pos = {
                    (SCREEN_WIDTH - inst->w) / 2,
                    score_pos.y + 40
                };
                SDL_BlitSurface(inst, NULL, screen, &inst_pos);
                SDL_FreeSurface(inst);
                
                // Check for ENTER key to return to menu
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                    gameState = STATE_MENU;
                    menuState = MAIN_MENU;
                      // Reset player
                    liberer_joueur(&joueur);
                    initialiser_joueur(&joueur, "Player1", PLAYER_SPRITE_PATH);
                }
                break;
        }

        // Update the screen
        SDL_Flip(screen);
        
        // Cap the frame rate
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    if (collision_mask) SDL_FreeSurface(collision_mask);
    
    cleanup_menu();
    
    TTF_CloseFont(font);
    if (music) Mix_FreeMusic(music);
    SDL_FreeSurface(background);
    
    // Free game elements
    liberer_joueur(&joueur);
    freeEnemy(&enemy);
    free_minimap(&mini);
    free_surface_enigme(&en);
    
    cleanup_SDL();

    return 0;
}

// Initialize SDL subsystems
int init_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        return 0;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur d'initialisation de SDL_mixer: %s\n", Mix_GetError());
        return 0;
    }

    if (TTF_Init() == -1) {
        printf("Erreur d'initialisation de SDL_ttf: %s\n", TTF_GetError());
        return 0;
    }
    
    return 1;
}

// Clean up SDL subsystems
void cleanup_SDL() {
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
}

// Load image with error handling (wrapper around our asset loader)
SDL_Surface* load_image(const char* filename) {
    // Simply forward to the standardized asset loader
    return load_asset_image(filename);
}

// Display player stats on screen
void display_stats(Joueur joueur, SDL_Surface *screen, TTF_Font *font) {
    SDL_Surface *text_surface;
    SDL_Rect position;
    SDL_Color white = {255, 255, 255};
    char stats_text[100];
    
    // Create stats string
    sprintf(stats_text, "Vie: %d   Score: %d", joueur.lives, joueur.score);
    
    // Render text
    text_surface = TTF_RenderText_Solid(font, stats_text, white);
    
    // Position at top left
    position.x = 10;
    position.y = 10;
    
    // Draw text
    SDL_BlitSurface(text_surface, NULL, screen, &position);
    
    // Free surface
    SDL_FreeSurface(text_surface);
}
