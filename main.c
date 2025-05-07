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
#include "joueur.h" // Add joueur header

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

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer Error: %s\n", Mix_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Set up screen
    SDL_Surface *screen = SDL_SetVideoMode(1600, 800, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
    if (!screen) {
        fprintf(stderr, "Screen Init Error: %s\n", SDL_GetError());
        Mix_CloseAudio();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_WM_SetCaption("Game", NULL);

    // Initialize Enigme
    SDL_Color white = {255, 255, 255};
    enigme en;
    init_enigme(&en, white);

    // Load font for stats display
    TTF_Font *statsFont = TTF_OpenFont("assets/font.ttf", 24);
    if (!statsFont) {
        fprintf(stderr, "Font loading error: %s\n", TTF_GetError());
        // Continue anyway, will skip stats display
    }

    // Load assets
    SDL_Surface *background = IMG_Load("map1.png");
    SDL_Surface *mask = IMG_Load("map1_masked.png");
    if (!background || !mask) {
        fprintf(stderr, "Error loading background or mask image.\n");
        free_surface_enigme(&en);
        if (statsFont) TTF_CloseFont(statsFont);
        Mix_CloseAudio();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialize player (joueur) entity
    Joueur joueur;
    initialiser_joueur(&joueur, "Player1", "assets/player_spritesheet.png");

    // For multiplayer mode
    Joueur joueur2;
    int multiplayerMode = 0; // Set to 1 to enable second player

    // Character selection menu
    CharacterSelectMenu characterMenu;
    init_character_select(&characterMenu);
    int characterSelectDone = 0;

    // Old minimap player representation (keeping for compatibility)
    Personne p, pM, pMprochaine;
    p.sprite = IMG_Load("perso1.png");
    pM.sprite = IMG_Load("perso2.png");
    if (!p.sprite || !pM.sprite) {
        fprintf(stderr, "Error loading sprite images.\n");
        liberer_joueur(&joueur);
        if (multiplayerMode) liberer_joueur(&joueur2);
        free_character_select(&characterMenu);
        SDL_FreeSurface(background);
        SDL_FreeSurface(mask);
        free_surface_enigme(&en);
        if (statsFont) TTF_CloseFont(statsFont);
        Mix_CloseAudio();
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

    // Add a new menu state for character selection
    typedef enum {
        CHARACTER_SELECT = 6
    } AdditionalMenuState;
    
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
                    
                    // Modify to go to character select when Play is selected
                    if (current_menu == PLAY_MENU) {
                        current_menu = CHARACTER_SELECT;
                    }
                    break;
                    
                case CHARACTER_SELECT:
                    handle_character_select_input(&characterMenu, event, &characterSelectDone);
                    if (characterSelectDone == 1) {
                        // Character selected, initialize player with selection
                        joueur = select_character(characterMenu, characterMenu.selectedCharacter, 
                                               characterMenu.selectedClothing);
                        if (multiplayerMode) {
                            // For multiplayer, second player defaults to next character
                            int nextChar = (characterMenu.selectedCharacter + 1) % 4;
                            joueur2 = select_character(characterMenu, nextChar, characterMenu.selectedClothing);
                        }
                        current_menu = PLAY_MENU;
                        characterSelectDone = 0;
                    } else if (characterSelectDone == -1) {
                        // Selection cancelled
                        current_menu = MAIN_MENU;
                        characterSelectDone = 0;
                    }
                    break;
                    
                case PLAY_MENU:
                    current_menu = handle_play_menu_events(&event);
                    
                    // Process jouer specific inputs
                    if (event.type == SDL_KEYDOWN) {
                        switch (event.key.keysym.sym) {
                            case SDLK_SPACE:
                                sauter_joueur(&joueur);
                                break;
                            case SDLK_LSHIFT:
                                // Start running instead of walking
                                // Will be handled in main game loop
                                break;
                            case SDLK_LCTRL:
                                attaquer_joueur(&joueur);
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
                
            case CHARACTER_SELECT:
                display_character_select(characterMenu, screen);
                break;
                
            case PLAY_MENU:
                // Original background and minimap display
                SDL_BlitSurface(background, NULL, screen, &position_BG);
                MAJMinimap(p.position_perso, &m, camera, redim);
                afficherminimap(m, screen);
                
                // Process player movement input
                sens = 0;
                const Uint8 *keys = SDL_GetKeyState(NULL);
                
                // Handle joueur movement
                if (keys[SDLK_RIGHT]) {
                    if (keys[SDLK_LSHIFT]) {
                        courir_joueur(&joueur, RIGHT, mask);
                    } else {
                        marcher_joueur(&joueur, RIGHT, mask);
                    }
                    sens = 1;  // For backward compatibility
                }
                
                if (keys[SDLK_LEFT]) {
                    if (keys[SDLK_LSHIFT]) {
                        courir_joueur(&joueur, LEFT, mask);
                    } else {
                        marcher_joueur(&joueur, LEFT, mask);
                    }
                    sens = -1;  // For backward compatibility
                }
                
                // If no horizontal movement is happening, set to IDLE
                if (!keys[SDLK_RIGHT] && !keys[SDLK_LEFT] && 
                    joueur.state != JUMPING && joueur.state != ATTACKING) {
                    set_player_state(&joueur, IDLE);
                }
                
                // Animate the player
                animer_joueur(&joueur);
                
                // Apply gravity and jumping physics
                if (joueur.jumpVelocity != 0 || !joueur.onGround) {
                    deplacer_joueur(&joueur, 0, joueur.jumpVelocity, mask);
                    joueur.jumpVelocity += 1; // Apply gravity
                    
                    // Check if player has landed
                    if (joueur.jumpVelocity > 0) {
                        // Check collision with ground (simplified)
                        if (collision_joueur_obstacle(joueur, mask)) {
                            joueur.jumpVelocity = 0;
                            joueur.onGround = 1;
                            if (joueur.state == JUMPING) {
                                set_player_state(&joueur, IDLE);
                            }
                        }
                    }
                }
                
                // Handle second player in multiplayer mode
                if (multiplayerMode) {
                    // Similar code for joueur2 would go here
                    // Using different keys for the second player
                }

                // Original minimap and camera update compatibility
                mouvement(&p, &pM, &pMprochaine, distance, longueur, longueurM, mask, sens);
                
                if (sens != 0) {
                    camera.x = pM.position_perso.x - (screen->w / 2);
                    if (camera.x < 0) camera.x = 0;
                    if (camera.x > longueurM - screen->w) camera.x = longueurM - screen->w;
                    
                    // Sync camera with joueur entity
                    joueur.camera = camera;
                }
                
                // Display player
                afficher_joueur(joueur, screen);
                
                // Display second player if in multiplayer mode
                if (multiplayerMode) {
                    afficher_joueur(joueur2, screen);
                }
                
                // Display stats if font loaded
                if (statsFont) {
                    display_stats(joueur, screen, statsFont);
                }
                
                // Keep minimap display compatibility
                SDL_BlitSurface(p.sprite, NULL, screen, &p.position_perso);
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
    liberer_joueur(&joueur);
    if (multiplayerMode) liberer_joueur(&joueur2);
    free_character_select(&characterMenu);
    SDL_FreeSurface(background);
    SDL_FreeSurface(mask);
    SDL_FreeSurface(p.sprite);
    SDL_FreeSurface(pM.sprite);
    free_surface_enigme(&en);
    free_minimap(&m);
    if (statsFont) TTF_CloseFont(statsFont);

    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();

    return 0;
}