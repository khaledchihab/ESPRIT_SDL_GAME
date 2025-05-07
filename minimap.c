#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "minimap.h"
void initmap(minimap *m)
{
    m->position_mini.x = 100;
    m->position_mini.y = 0;
    m->sprite = NULL;
    m->bonhomme_mini = NULL;
    
    // Load minimap background
    m->sprite = IMG_Load("minimap.png");
    if (m->sprite == NULL) {
        fprintf(stderr, "Failed to load minimap.png: %s\n", IMG_GetError());
        // Provide a fallback - create a simple gray rectangle
        m->sprite = SDL_CreateRGBSurface(SDL_SWSURFACE, 200, 150, 32, 0, 0, 0, 0);
        if (m->sprite != NULL) {
            SDL_FillRect(m->sprite, NULL, SDL_MapRGB(m->sprite->format, 200, 200, 200));
        }
    }
    
    // Create a small red dot for player representation on minimap
    m->bonhomme_mini = SDL_CreateRGBSurface(SDL_SWSURFACE, 8, 8, 32, 0, 0, 0, 0);
    
    if (m->bonhomme_mini != NULL) {
        // Fill the surface with red color
        SDL_FillRect(m->bonhomme_mini, NULL, SDL_MapRGB(m->bonhomme_mini->format, 255, 0, 0));
    } else {
        fprintf(stderr, "Failed to create player indicator for minimap\n");
    }
    
    // Initialize player position on minimap
    m->position_bonhomme.x = 0;
    m->position_bonhomme.y = 0;
    m->position_bonhomme.h = 8; // Height of our dot
    m->position_bonhomme.w = 8; // Width of our dot
}
void afficherminimap (minimap m, SDL_Surface * screen)
{
    // Display the minimap background
    SDL_BlitSurface(m.sprite, NULL, screen, &m.position_mini);
    
    // Display the player representation (bonhomme) on the minimap
    SDL_BlitSurface(m.bonhomme_mini, NULL, screen, &m.position_bonhomme);
}
void free_minimap (minimap *m)
{
    // Free minimap background
    SDL_FreeSurface(m->sprite);
    
    // Free player representation on minimap
    SDL_FreeSurface(m->bonhomme_mini);
}
SDL_Color GetPixel(SDL_Surface *Background, int x, int y)
{
SDL_Color color;
Uint32 col = 0;
char *pixelPosition = (char*)Background->pixels;
pixelPosition += (Background->pitch * y);
pixelPosition += (Background->format->BytesPerPixel * x);
memcpy(&col, pixelPosition, Background->format->BytesPerPixel);
SDL_GetRGB(col, Background->format, &color.r, &color.g, &color.b);
return (color);
}
int collisionPP( Personne p, SDL_Surface * Masque)
{
SDL_Color test ,couleurnoir= {0,0,0};
SDL_Rect pos[8];
pos[0].x=p.position_perso.x;
pos[0].y=p.position_perso.y;
pos[1].x=p.position_perso.x+p.position_perso.w/2;
pos[1].y=p.position_perso.y;
pos[2].x=p.position_perso.x+p.position_perso.w;
pos[2].y=p.position_perso.y;
pos[3].x=p.position_perso.x;
pos[3].y=p.position_perso.y+p.position_perso.h/2;
pos[4].x=p.position_perso.x;
pos[4].y=p.position_perso.y+p.position_perso.h;
pos[5].x=p.position_perso.x+p.position_perso.w/2;
pos[5].y=p.position_perso.y+p.position_perso.h;
pos[6].x=p.position_perso.x+p.position_perso.w;
pos[6].y=p.position_perso.y+p.position_perso.h;
pos[7].x=p.position_perso.x+p.position_perso.w;
pos[7].y=p.position_perso.y+p.position_perso.h/2;
int collision=0 , x ,y ;
for(int i=0 ;i<8 && collision==0;i++)
{
x=pos[i].x;
y=pos[i].y;
test=GetPixel(Masque,x,y);
if(test.r==0 && test.g==0 && test.b==0)
collision=1;
}
return collision;
}
void update_1_(SDL_Rect *prochain , SDL_Rect *principal, SDL_Rect *Pminimap, int distance, int longueur,int longueurM)
{
principal->x=prochain->x;
Pminimap->x+=distance*longueur/longueurM;
}
void update_2_(SDL_Rect *prochain , SDL_Rect *principal, SDL_Rect *Pminimap, int distance, int longueur,int longueurM)
{
principal->x=prochain->x;
Pminimap->x-=distance*longueur/longueurM;
}
void mouvement(Personne *p ,Personne *pM ,Personne *pMprochaine, int distance, int longueur,int longueurM , SDL_Surface *masked , int sens)
{
if(sens==1)
{
pMprochaine->position_perso.x+=distance;
if(pMprochaine->position_perso.x >=longueurM)
{
pMprochaine->position_perso.x=longueurM;
pM->position_perso.x=longueurM;
p->position_perso.x=longueur+95;
}
else if(collisionPP( *pMprochaine, masked)==0)
{
pM->position_perso.x=pMprochaine->position_perso.x;
p->position_perso.x+=distance*longueur/longueurM;
}
else{
pMprochaine->position_perso.x=pM->position_perso.x;
}
}
else if(sens==-1)
{
pMprochaine->position_perso.x-=distance;
if(pMprochaine->position_perso.x <=0)
{
pMprochaine->position_perso.x=0;
pM->position_perso.x=0;
p->position_perso.x=95;
}
else if(collisionPP( *pMprochaine, masked)==0 )
{
pM->position_perso.x=pMprochaine->position_perso.x;
p->position_perso.x-=distance*longueur/longueurM;
}
else{
pMprochaine->position_perso.x=pM->position_perso.x;
}
}
}

void MAJMinimap(SDL_Rect posJoueur, minimap * m, SDL_Rect camera, int redimensionnement)
{
    // Calculate absolute position of player
    int posJoueurABS_x = posJoueur.x + camera.x;
    int posJoueurABS_y = posJoueur.y + camera.y;
    
    // Apply redimensionnement (scaling) to get minimap coordinates
    m->position_bonhomme.x = m->position_mini.x + (posJoueurABS_x * redimensionnement / 100);
    m->position_bonhomme.y = m->position_mini.y + (posJoueurABS_y * redimensionnement / 100);
}







