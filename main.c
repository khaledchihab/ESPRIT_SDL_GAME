
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "enigme.h"
#include <string.h>
#include <math.h>

int main(int argc,char **argv[])
{
    SDL_Surface *screen =NULL;
    SDL_Init(SDL_INIT_EVERYTHING);
    screen=SDL_SetVideoMode(1000,800, 32,SDL_HWSURFACE| SDL_DOUBLEBUF);
    SDL_WM_SetCaption("game",NULL);
    TTF_Init();
    SDL_Color Color={255,255,255};
    enigme en;
    init_enigme(&en,Color);

    int res = quiz1(&en,screen );
    
    free_surface_enigme( &en );
    SDL_Quit();

    return EXIT_SUCCESS ;
}

