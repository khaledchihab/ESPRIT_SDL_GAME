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
#include "joueur.h" // Add joueur header

int main()
{
    SDL_Surface *ecran = NULL;
    SDL_Surface *texte = NULL;

    SDL_Event event;
    int continuer = 1;
    int state = 0;

    int choixMenu = 0;

    // SDL initialization
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();
    ecran = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_WM_SetCaption("Mon jeu SDL", NULL);

    // Fonts
    TTF_Font *police = TTF_OpenFont("assets/police.ttf", 30);
    SDL_Color couleurBlanche = {255, 255, 255};

    // Musique
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
    Mix_Music *musique = Mix_LoadMUS("assets/drumbeat.wav");
    Mix_PlayMusic(musique, -1);

    // Menu loop
    choixMenu = afficherMenu(ecran);

    if (choixMenu == 1) // Jouer
    {
        // Gameplay logic or transition to submenu
    }
    else if (choixMenu == 2) // Options
    {
        // Options screen
    }
    else if (choixMenu == 3) // Scores
    {
        // High scores
    }
    else if (choixMenu == 4) // Histoire
    {
        // Story mode or cutscene
    }
    else if (choixMenu == 5) // Quitter
    {
        continuer = 0;
    }

    // Clean up
    TTF_CloseFont(police);
    TTF_Quit();
    Mix_FreeMusic(musique);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}
