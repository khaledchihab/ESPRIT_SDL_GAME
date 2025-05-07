#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "menu.h"
#include "minimap.h"

int main(int argc, char *argv[]) {
    // Initialize SDL and subsystems
    if (!init_SDL()) {
        printf("Failed to initialize SDL!\n");
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        return 1;
    }

    if (!load_assets()) {
        printf("Failed to load assets!\n");
        cleanup();
        return 1;
    }

    // Setup screen
    SDL_Surface *screen = SDL_SetVideoMode(1600, 800, 32, SDL_SWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
    if (screen == NULL) {
        printf("Screen Error: %s\n", SDL_GetError());
        return 1;
    }

    // Load minimap elements
    minimap m;
    initmap(&m);

    SDL_Surface *imageDeFond = IMG_Load("map1.png");
    if (!imageDeFond) {
        fprintf(stderr, "Failed to load map1.png: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Surface *masked = IMG_Load("map1_masked.png");
    if (!masked) {
        fprintf(stderr, "Failed to load map1_masked.png: %s\n", IMG_GetError());
        SDL_FreeSurface(imageDeFond);
        free_minimap(&m);
        return 1;
    }

    Personne p, pM, pMprochaine;
    p.sprite = IMG_Load("perso1.png");
    if (!p.sprite) {
        fprintf(stderr, "Failed to load perso1.png: %s\n", IMG_GetError());
        return 1;
    }
    p.position_perso.x = 95;
    p.position_perso.y = 35;

    pM.sprite = IMG_Load("perso2.png");
    if (!pM.sprite) {
        fprintf(stderr, "Failed to load perso2.png: %s\n", IMG_GetError());
        return 1;
    }
    pM.position_perso.x = 0;
    pM.position_perso.y = 350;

    pMprochaine.position_perso.x = pM.position_perso.x;
    pMprochaine.position_perso.y = pM.position_perso.y;
    pMprochaine.position_perso.w = 137;
    pMprochaine.position_perso.h = 357;

    SDL_Rect camera = {0, 0, screen->w, screen->h};
    SDL_Rect position_BG = {0, 0};

    SDL_Event event;
    int quit = 0;
    int continuer = 1;
    int sens = 0, distance = 10;
    int redimensionnement = 20;
    int longueurM = 6268, largeurM = 800;
    int longueur = 1254, largeur = 160;

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
                    // Fall through to game logic
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

        // Render based on menu
        switch (current_menu) {
            case MAIN_MENU:
                render_main_menu(screen);
                break;

            case PLAY_MENU:
                sens = 0;

                SDL_BlitSurface(imageDeFond, NULL, screen, &position_BG);
                MAJMinimap(p.position_perso, &m, camera, redimensionnement);
                afficherminimap(m, screen);
                SDL_BlitSurface(p.sprite, NULL, screen, &p.position_perso);
                SDL_BlitSurface(pM.sprite, NULL, screen, &pM.position_perso);

                const Uint8 *keys = SDL_GetKeyState(NULL);
                if (keys[SDLK_RIGHT]) sens = 1;
                if (keys[SDLK_LEFT]) sens = -1;

                mouvement(&p, &pM, &pMprochaine, distance, longueur, longueurM, masked, sens);

                if (sens != 0) {
                    camera.x = pM.position_perso.x - (screen->w / 2);
                    if (camera.x < 0) camera.x = 0;
                    if (camera.x > longueurM - screen->w) camera.x = longueurM - screen->w;
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

    // Cleanup resources
    free_minimap(&m);
    SDL_FreeSurface(p.sprite);
    SDL_FreeSurface(pM.sprite);
    SDL_FreeSurface(imageDeFond);
    SDL_FreeSurface(masked);
    cleanup();
    SDL_Quit();
    return 0;
}
