#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

#include "menu.h"
#include "minimap.h"
#include "enigme.h"
#include "ennemie.h" // Added enemy header

int main(int argc, char *argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Set up screen
    SDL_Surface *screen = SDL_SetVideoMode(1600, 800, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
    if (!screen) {
        fprintf(stderr, "Screen Init Error: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_WM_SetCaption("Game", NULL);

    // Initialize Enigme
    SDL_Color white = {255, 255, 255};
    enigme en;
    init_enigme(&en, white);

    // Load assets
    SDL_Surface *background = IMG_Load("map1.png");
    SDL_Surface *mask = IMG_Load("map1_masked.png");
    if (!background || !mask) {
        fprintf(stderr, "Error loading background or mask image.\n");
        free_surface_enigme(&en);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    Personne p, pM, pMprochaine;
    p.sprite = IMG_Load("perso1.png");
    pM.sprite = IMG_Load("perso2.png");
    if (!p.sprite || !pM.sprite) {
        fprintf(stderr, "Error loading sprite images.\n");
        SDL_FreeSurface(background);
        SDL_FreeSurface(mask);
        free_surface_enigme(&en);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Initial positions
    p.position_perso.x = 95;
    p.position_perso.y = 35;

    pM.position_perso.x = 0;
    pM.position_perso.y = 350;

    pMprochaine.position_perso = pM.position_perso;
    pMprochaine.position_perso.w = 137;
    pMprochaine.position_perso.h = 357;

    // Initialize enemies
    Enemy enemy1, enemy2;
    initEnemy(&enemy1, LEVEL1, 300, 200); // Level 1 enemy at position 300, 200
    initEnemy(&enemy2, LEVEL2, 500, 400); // Level 2 enemy at position 500, 400

    // Initialize minimap
    minimap m;
    initmap(&m);

    // Setup camera
    SDL_Rect camera = {0, 0, screen->w, screen->h};
    SDL_Rect position_BG = {0, 0};

    SDL_Event event;
    int quit = 0;
    int sens = 0, distance = 10;
    int redim = 20;
    int longueurM = 6268;
    int longueur = 1254;
    
    // Player health variables
    int player_health = 100;
    int player_damaged_timer = 0;
    int invincibility_time = 30; // Frames of invincibility after being hit

    current_menu = MAIN_MENU;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }

            switch (current_menu) {
                case MAIN_MENU:
                    current_menu = handle_main_menu_events(&event);
                    if (current_menu == QUIT) quit = 1;
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

        switch (current_menu) {
            case MAIN_MENU:
                render_main_menu(screen);
                break;
            case PLAY_MENU:
                sens = 0;
                SDL_BlitSurface(background, NULL, screen, &position_BG);
                
                // Handle player movement
                const Uint8 *keys = SDL_GetKeyState(NULL);
                if (keys[SDLK_RIGHT]) sens = 1;
                if (keys[SDLK_LEFT]) sens = -1;
                
                mouvement(&p, &pM, &pMprochaine, distance, longueur, longueurM, mask, sens);
                
                // Update camera position
                if (sens != 0) {
                    camera.x = pM.position_perso.x - (screen->w / 2);
                    if (camera.x < 0) camera.x = 0;
                    if (camera.x > longueurM - screen->w) camera.x = longueurM - screen->w;
                }
                
                // Update minimap
                MAJMinimap(p.position_perso, &m, camera, redim);
                
                // Update and animate enemies
                animateEnemy(&enemy1);
                animateEnemy(&enemy2);
                
                // Move enemies with AI
                moveEnemyAI(&enemy1, pM.position_perso, mask);
                moveEnemyAI(&enemy2, pM.position_perso, mask);
                
                // Check for collisions with player
                if (player_damaged_timer <= 0) {
                    int enemy1_collision = checkCollisionWithPlayer(&enemy1, pM.position_perso);
                    int enemy2_collision = checkCollisionWithPlayer(&enemy2, pM.position_perso);
                    
                    if (enemy1_collision) {
                        player_health -= (enemy1_collision == 1) ? enemy1.damage : enemy1.damage * 2; // Double damage for ES collision
                        player_damaged_timer = invincibility_time;
                    }
                    
                    if (enemy2_collision) {
                        player_health -= (enemy2_collision == 1) ? enemy2.damage : enemy2.damage * 2;
                        player_damaged_timer = invincibility_time;
                    }
                    
                    // Check if player is dead
                    if (player_health <= 0) {
                        player_health = 0;
                        // Handle game over here
                        current_menu = MAIN_MENU;
                    }
                } else {
                    player_damaged_timer--;
                }
                
                // Player attack (with SPACE key)
                if (keys[SDLK_SPACE]) {
                    // Simple proximity-based attack
                    SDL_Rect attack_range = pM.position_perso;
                    attack_range.x -= 30;  // Extend attack range in all directions
                    attack_range.y -= 30;
                    attack_range.w += 60;
                    attack_range.h += 60;
                    
                    // Check if enemies are within attack range
                    if (checkCollisionWithPlayer(&enemy1, attack_range)) {
                        damageEnemy(&enemy1, 10);
                    }
                    
                    if (checkCollisionWithPlayer(&enemy2, attack_range)) {
                        damageEnemy(&enemy2, 10);
                    }
                }
                
                // Render everything
                // Display player health
                SDL_Rect health_bg = {10, 10, 200, 20};
                SDL_Rect health_fg = {10, 10, (player_health * 200) / 100, 20};
                SDL_FillRect(screen, &health_bg, SDL_MapRGB(screen->format, 255, 0, 0)); // Red background
                SDL_FillRect(screen, &health_fg, SDL_MapRGB(screen->format, 0, 255, 0)); // Green foreground
                
                // Draw minimap, player, and enemies
                afficherminimap(m, screen);
                displayEnemy(screen, &enemy1);
                displayEnemy(screen, &enemy2);
                SDL_BlitSurface(p.sprite, NULL, screen, &p.position_perso);
                SDL_BlitSurface(pM.sprite, NULL, screen, &pM.position_perso);
                
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

        SDL_Flip(screen);
        SDL_Delay(10);
    }

    // Free resources
    SDL_FreeSurface(background);
    SDL_FreeSurface(mask);
    SDL_FreeSurface(p.sprite);
    SDL_FreeSurface(pM.sprite);
    freeEnemy(&enemy1);
    freeEnemy(&enemy2);
    free_surface_enigme(&en);
    free_minimap(&m);

    TTF_Quit();
    SDL_Quit();

    return 0;
}