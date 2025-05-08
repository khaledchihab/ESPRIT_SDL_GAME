#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "minimap.h"
#include "assets.h"

// Improve minimap initialization with proper error handling and scaling
void initmap(minimap *m) {
    // Load minimap background
    m->backgroundMini = load_asset_image(TEXTURE_PATH "minibg.png");
    if (!m->backgroundMini) {
        printf("Failed to load minimap background: %s\n", IMG_GetError());
        // Create fallback background
        m->backgroundMini = SDL_CreateRGBSurface(SDL_SWSURFACE, 200, 150, 32, 0, 0, 0, 0);
        SDL_FillRect(m->backgroundMini, NULL, SDL_MapRGB(m->backgroundMini->format, 50, 50, 50));
    }
    
    // Load player indicator
    m->joueurMini = load_asset_image(TEXTURE_PATH "minijoueur.png");
    if (!m->joueurMini) {
        printf("Failed to load minimap player: %s\n", IMG_GetError());
        // Create fallback player indicator
        m->joueurMini = SDL_CreateRGBSurface(SDL_SWSURFACE, 10, 10, 32, 0, 0, 0, 0);
        SDL_FillRect(m->joueurMini, NULL, SDL_MapRGB(m->joueurMini->format, 255, 0, 0));
    }
    
    // Initialize positions
    m->miniPos.x = SCREEN_WIDTH - m->backgroundMini->w - 10;
    m->miniPos.y = 10;
    m->miniPos.h = m->backgroundMini->h;
    m->miniPos.w = m->backgroundMini->w;
    
    // Initialize player position on minimap
    m->posMiniJoueur.x = m->miniPos.x + 10; // Default starting position
    m->posMiniJoueur.y = m->miniPos.y + m->backgroundMini->h / 2;
    m->posMiniJoueur.h = m->joueurMini->h;
    m->posMiniJoueur.w = m->joueurMini->w;
}

// Display minimap with proper blending and transparency
void afficherminimap(minimap m, SDL_Surface *screen) {
    // Set alpha blending for semi-transparent minimap
    SDL_SetAlpha(m.backgroundMini, SDL_SRCALPHA, 180); // 180 = semi-transparent
    
    // Blit minimap background
    SDL_BlitSurface(m.backgroundMini, NULL, screen, &m.miniPos);
    
    // Blit player indicator (full opacity)
    SDL_SetAlpha(m.joueurMini, SDL_SRCALPHA, 255); // 255 = fully opaque
    SDL_BlitSurface(m.joueurMini, NULL, screen, &m.posMiniJoueur);
    
    // Draw a border around the minimap
    SDL_Rect border = m.miniPos;
    border.x--;
    border.y--;
    border.w += 2;
    border.h += 2;
    
    // Use a function to draw only the border edges (not fill)
    drawRect(screen, border, SDL_MapRGB(screen->format, 255, 255, 255));
}

// Helper function to draw just the outline of a rectangle
void drawRect(SDL_Surface *screen, SDL_Rect rect, Uint32 color) {
    // Draw top line
    SDL_Rect line = {rect.x, rect.y, rect.w, 1};
    SDL_FillRect(screen, &line, color);
    
    // Draw bottom line
    line.y = rect.y + rect.h - 1;
    SDL_FillRect(screen, &line, color);
    
    // Draw left line
    line.y = rect.y;
    line.w = 1;
    line.h = rect.h;
    SDL_FillRect(screen, &line, color);
    
    // Draw right line
    line.x = rect.x + rect.w - 1;
    SDL_FillRect(screen, &line, color);
}

// Free memory used by minimap
void free_minimap(minimap *m) {
    if (m->backgroundMini) {
        SDL_FreeSurface(m->backgroundMini);
        m->backgroundMini = NULL;
    }
    
    if (m->joueurMini) {
        SDL_FreeSurface(m->joueurMini);
        m->joueurMini = NULL;
    }
}

// Update minimap to correctly track player position
void MAJMinimap(SDL_Rect posJoueur, minimap *m, SDL_Rect camera, float redimensionnement) {
    // Calculate scaling factors between game world and minimap
    // These scale factors convert game world coordinates to minimap coordinates
    float scaleX = (float)m->backgroundMini->w / SCREEN_WIDTH;
    float scaleY = (float)m->backgroundMini->h / SCREEN_HEIGHT;
    
    // Calculate new player position on minimap
    // Add camera offset to make the minimap show the visible area
    int miniX = m->miniPos.x + (posJoueur.x - camera.x) * scaleX;
    int miniY = m->miniPos.y + (posJoueur.y - camera.y) * scaleY;
    
    // Apply redimensionnement factor if needed
    miniX = (int)(miniX / redimensionnement);
    miniY = (int)(miniY / redimensionnement);
    
    // Ensure player indicator stays within minimap bounds
    if (miniX < m->miniPos.x) miniX = m->miniPos.x;
    if (miniY < m->miniPos.y) miniY = m->miniPos.y;
    if (miniX > m->miniPos.x + m->backgroundMini->w - m->joueurMini->w)
        miniX = m->miniPos.x + m->backgroundMini->w - m->joueurMini->w;
    if (miniY > m->miniPos.y + m->backgroundMini->h - m->joueurMini->h)
        miniY = m->miniPos.y + m->backgroundMini->h - m->joueurMini->h;
    
    // Update player position on minimap
    m->posMiniJoueur.x = miniX;
    m->posMiniJoueur.y = miniY;
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







