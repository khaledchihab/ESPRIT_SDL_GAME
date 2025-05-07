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
#include "background.h"

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

    // Initialize Background
    Background bg;
    initBackground(&bg, "map1.png", 1); // 1 for mobile background
    
    // Initialize game time
    GameTime gameTime;
    initGameTime(&gameTime, "times.ttf", 24); // Assuming you have this font in your project directory

    // Load assets
    SDL_Surface *mask = IMG_Load("map1_masked.png");
    if (!bg.img || !mask) {
        fprintf(stderr, "Error loading background or mask image.\n");
        free_surface_enigme(&en);
        freeBackground(&bg);
        freeGameTime(&gameTime);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    Personne p, pM, pMprochaine;
    p.sprite = IMG_Load("perso1.png");
    pM.sprite = IMG_Load("perso2.png");
    if (!p.sprite || !pM.sprite) {
        fprintf(stderr, "Error loading sprite images.\n");
        SDL_FreeSurface(mask);
        free_surface_enigme(&en);
        freeBackground(&bg);
        freeGameTime(&gameTime);
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
    int scroll_speed = 5;

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
                    
                    // Add background scrolling with keyboard controls
                    if (event.type == SDL_KEYDOWN) {
                        switch (event.key.keysym.sym) {
                            case SDLK_UP:
                                scrollBackground(&bg, 0, scroll_speed);
                                break;
                            case SDLK_DOWN:
                                scrollBackground(&bg, 1, scroll_speed);
                                break;
                            case SDLK_LEFT:
                                scrollBackground(&bg, 2, scroll_speed);
                                break;
                            case SDLK_RIGHT:
                                scrollBackground(&bg, 3, scroll_speed);
                                break;
                            default:
                                break;
                        }
                    }
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
                
                // Display the background using our new module
                displayBackground(bg, screen);
                
                MAJMinimap(p.position_perso, &m, camera, redim);
                afficherminimap(m, screen);
                SDL_BlitSurface(p.sprite, NULL, screen, &p.position_perso);
                SDL_BlitSurface(pM.sprite, NULL, screen, &pM.position_perso);
                
                // Update and display game time
                updateGameTime(&gameTime, screen);

                const Uint8 *keys = SDL_GetKeyState(NULL);
                if (keys[SDLK_RIGHT]) sens = 1;
                if (keys[SDLK_LEFT]) sens = -1;

                mouvement(&p, &pM, &pMprochaine, distance, longueur, longueurM, mask, sens);

                if (sens != 0) {
                    camera.x = pM.position_perso.x - (screen->w / 2);
                    if (camera.x < 0) camera.x = 0;
                    if (camera.x > longueurM - screen->w) camera.x = longueurM - screen->w;
                    
                    // Sync background camera with movement
                    bg.camera_pos.x = camera.x;
                }
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
    SDL_FreeSurface(mask);
    SDL_FreeSurface(p.sprite);
    SDL_FreeSurface(pM.sprite);
    free_surface_enigme(&en);
    free_minimap(&m);
    freeBackground(&bg);
    freeGameTime(&gameTime);

    TTF_Quit();
    SDL_Quit();

    return 0;
}