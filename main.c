#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

#include "headers/menu.h"
#include "headers/player.h"
#include "headers/enigme.h"
#include "headers/enemy.h"
#include "headers/mini_map.h"

// Enum for managing game states
typedef enum {
    STATE_MAIN_GAME,
    STATE_ENIGME1,
    STATE_ENIGME2
} GameState;

int main(int argc, char *argv[]) {
    SDL_Surface *screen = NULL;
    SDL_Surface *background = NULL;
    SDL_Rect bgPos = {0, 0};

    TTF_Font *font = NULL;
    Mix_Music *music = NULL;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur d'initialisation de SDL_mixer: %s\n", Mix_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("Erreur d'initialisation de SDL_ttf: %s\n", TTF_GetError());
        return 1;
    }

    screen = SDL_SetVideoMode(1280, 720, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (screen == NULL) {
        printf("Erreur lors de la création de la fenêtre: %s\n", SDL_GetError());
        return 1;
    }

    SDL_WM_SetCaption("Mon Jeu", NULL);

    font = TTF_OpenFont("assets/font.ttf", 24);
    if (font == NULL) {
        printf("Erreur lors du chargement de la police: %s\n", TTF_GetError());
        return 1;
    }

    music = Mix_LoadMUS("assets/drumloop.wav");
    if (music == NULL) {
        printf("Erreur lors du chargement de la musique: %s\n", Mix_GetError());
        return 1;
    }
    Mix_PlayMusic(music, -1); // Play in loop

    background = IMG_Load("assets/background1.png");

    // Init game elements
    Player player;
    initPlayer(&player);

    Enemy enemy;
    initEnemy(&enemy);

    MiniMap miniMap;
    initMiniMap(&miniMap);

    Enigme enigme;
    initEnigme(&enigme, "assets/enigme.txt", "assets/backgroundenig.png", font);

    // Variables
    SDL_Event event;
    int continuer = 1;
    int current_menu = MAIN_MENU;
    GameState gameState = STATE_MAIN_GAME;

    // Triggers
    int enigme1_trigger_x = 500;
    int enigme2_trigger_x = 1000;
    int trigger_range = 50;

    while (continuer) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                continuer = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_j: current_menu = PLAY_MENU; break;
                    case SDLK_o: current_menu = OPTIONS_MENU; break;
                    case SDLK_m: current_menu = SCORE_MENU; break;
                    case SDLK_ESCAPE: continuer = 0; break;
                    case SDLK_e:
                        if (gameState == STATE_MAIN_GAME) {
                            gameState = STATE_ENIGME2;
                        }
                        break;
                }
            }

            // Menu-specific event handling
            switch (current_menu) {
                case MAIN_MENU:
                    current_menu = handle_main_menu_events(&event);
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

        // State machine
        if (gameState == STATE_ENIGME1) {
            // Launch enigme1 code
            gameState = STATE_MAIN_GAME;
        } else if (gameState == STATE_ENIGME2) {
            int result = play_enigme2(screen);
            gameState = STATE_MAIN_GAME;
        } else {
            switch (current_menu) {
                case MAIN_MENU:
                    drawMenu(screen, font);
                    break;
                case PLAY_MENU: {
                    const Uint8 *keystate = SDL_GetKeyState(NULL);
                    updatePlayer(&player, keystate);
                    updateEnemy(&enemy, player.position.x);
                    updateMiniMap(&miniMap, player.position);

                    SDL_BlitSurface(background, NULL, screen, &bgPos);
                    drawPlayer(screen, &player);
                    drawEnemy(screen, &enemy);
                    drawMiniMap(screen, &miniMap);
                    drawHealthBar(screen, player.health, font);

                    // Enigme triggers
                    if (abs(player.position.x - enigme1_trigger_x) < trigger_range) {
                        gameState = STATE_ENIGME1;
                    }
                    if (abs(player.position.x - enigme2_trigger_x) < trigger_range) {
                        gameState = STATE_ENIGME2;
                    }

                    if (checkCollision(player.position, enemy.position)) {
                        player.health -= 5;
                    }

                    if (player.health <= 0) {
                        printf("Game Over\n");
                        continuer = 0;
                    }
                    break;
                }
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
        }

        SDL_Flip(screen);
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    TTF_CloseFont(font);
    Mix_FreeMusic(music);
    SDL_FreeSurface(background);
    cleanUpEnigme(&enigme);
    SDL_Quit();
    TTF_Quit();
    Mix_CloseAudio();

    return 0;
}
