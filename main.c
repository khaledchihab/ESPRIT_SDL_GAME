#include "headers/menu.h"
#include "headers/player.h"
#include "headers/enigme.h"
#include "headers/enemy.h"
#include "headers/mini_map.h"

int main(int argc, char *argv[]) {
    SDL_Surface *screen = NULL;
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
    Mix_PlayMusic(music, -1); // Joue la musique en boucle

    int menu = MAIN_MENU;
    int continuer = 1;
    SDL_Event event;

    Player player;
    initPlayer(&player);

    Enemy enemy;
    initEnemy(&enemy);

    MiniMap miniMap;
    initMiniMap(&miniMap);

    Enigme enigme;
    initEnigme(&enigme, "assets/enigme.txt", "assets/backgroundenig.png", font);

    SDL_Surface *background = IMG_Load("assets/background1.png");
    SDL_Rect bgPos = {0, 0};

    while (continuer) {
        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                continuer = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_j:
                        menu = PLAY_MENU;
                        break;
                    case SDLK_o:
                        menu = OPTIONS_MENU;
                        break;
                    case SDLK_m:
                        menu = SCORE_MENU;
                        break;
                    case SDLK_ESCAPE:
                        continuer = 0;
                        break;
                }
                break;
        }

        if (menu == MAIN_MENU) {
            drawMenu(screen, font);
        } else if (menu == PLAY_MENU) {
            const Uint8 *keystate = SDL_GetKeyState(NULL);
            updatePlayer(&player, keystate);
            updateEnemy(&enemy, player.position.x);
            updateMiniMap(&miniMap, player.position);

            SDL_BlitSurface(background, NULL, screen, &bgPos);
            drawPlayer(screen, &player);
            drawEnemy(screen, &enemy);
            drawMiniMap(screen, &miniMap);
            drawHealthBar(screen, player.health, font);

            if (checkCollision(player.position, enemy.position)) {
                player.health -= 5;
            }

            if (player.health <= 0) {
                printf("Game Over\n");
                continuer = 0;
            }
        }

        SDL_Flip(screen);
        SDL_Delay(16); // ~60 FPS
    }

    // Clean up
    TTF_CloseFont(font);
    Mix_FreeMusic(music);
    SDL_FreeSurface(background);
    cleanUpEnigme(&enigme);
    SDL_Quit();
    TTF_Quit();
    Mix_CloseAudio();

    return 0;
}