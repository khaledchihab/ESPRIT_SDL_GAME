#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "minimap.h"
int main()
{
int collision ,distance =10 , sens=0;
SDL_Event event;
minimap m;
Personne p , pM , pMprochaine;
SDL_Surface *screen = NULL,*imageDeFond=NULL , *masked=NULL;
SDL_Rect position_BG, camera;
int redimensionnement = 20; // 20% scaling factor for minimap as mentioned in prompt
int longueurM=6268 , largeurM=800 , longueur =1254 , largeur=160;
screen = SDL_SetVideoMode(1600, 800 , 32, SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE);
TTF_Init();
if(screen==NULL)
{
printf("ERREUR: %s\n", SDL_GetError());
return 1;
}
position_BG.x=0;
position_BG.y=0;

// Initialize camera for scrolling
camera.x = 0;
camera.y = 0;
camera.w = screen->w;
camera.h = screen->h;

// Load main background image
imageDeFond = IMG_Load("map1.png");
if (imageDeFond == NULL) {
    fprintf(stderr, "Failed to load map1.png: %s\n", IMG_GetError());
    return 1;
}

int continuer= 1;
initmap(&m);

// Load masked collision map
masked = IMG_Load("map1_masked.png");
if (masked == NULL) {
    fprintf(stderr, "Failed to load map1_masked.png: %s\n", IMG_GetError());
    SDL_FreeSurface(imageDeFond);
    free_minimap(&m);
    return 1;
}

// Load main player sprite
p.sprite = IMG_Load("perso1.png");
if (p.sprite == NULL) {
    fprintf(stderr, "Failed to load perso1.png: %s\n", IMG_GetError());
    SDL_FreeSurface(imageDeFond);
    SDL_FreeSurface(masked);
    free_minimap(&m);
    return 1;
}
p.position_perso.x = 95;
p.position_perso.y = 35;

// Load secondary character sprite
pM.sprite = IMG_Load("perso2.png");
if (pM.sprite == NULL) {
    fprintf(stderr, "Failed to load perso2.png: %s\n", IMG_GetError());
    SDL_FreeSurface(imageDeFond);
    SDL_FreeSurface(masked);
    SDL_FreeSurface(p.sprite);
    free_minimap(&m);
    return 1;
}
pM.position_perso.x=0;
pM.position_perso.y=350;
pMprochaine.position_perso.h=357;
pMprochaine.position_perso.w=137;
pMprochaine.position_perso.x=pM.position_perso.x;
pMprochaine.position_perso.y=pM.position_perso.y;

while (continuer)
{
    sens=0;
    SDL_BlitSurface(imageDeFond, NULL, screen, &position_BG);
    
    // Update minimap with player position
    MAJMinimap(p.position_perso, &m, camera, redimensionnement);
    
    // Display minimap and player on it
    afficherminimap(m, screen);
    
    SDL_BlitSurface(p.sprite, NULL, screen, &p.position_perso);
    SDL_BlitSurface(pM.sprite, NULL, screen, &pM.position_perso);
    
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        continuer = 0;
                        break;
                    case SDLK_RIGHT:
                        sens=1;
                        break;
                    case SDLK_LEFT:
                        sens=-1;
                        break;
                }
        }
    }
    
    mouvement(&p, &pM, &pMprochaine, distance, longueur, longueurM, masked, sens);
    
    // Update camera position based on character movement for scrolling effect
    if (sens != 0) {
        camera.x = pM.position_perso.x - (screen->w / 2);
        
        // Ensure camera stays within valid bounds
        if (camera.x < 0) camera.x = 0;
        if (camera.x > longueurM - screen->w) camera.x = longueurM - screen->w;
    }
    
    SDL_Flip(screen);
}

// Free resources
free_minimap(&m);
SDL_FreeSurface(p.sprite);
SDL_FreeSurface(pM.sprite);
SDL_FreeSurface(imageDeFond);
SDL_FreeSurface(masked);
SDL_FreeSurface(screen);
SDL_Quit();
return EXIT_SUCCESS;
}