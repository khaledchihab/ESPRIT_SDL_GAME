#ifndef MINIMAP_H_INCLUDED
#define MINIMAP_H_INCLUDED
typedef struct
{
SDL_Rect position_perso;
SDL_Surface *sprite;
}Personne;
typedef struct
{
SDL_Rect position_mini;
SDL_Surface *sprite;
SDL_Surface *bonhomme_mini; // Player representation on minimap
SDL_Rect position_bonhomme; // Position of player on minimap
}minimap;
void initmap( minimap *m);
void afficherminimap (minimap m, SDL_Surface * screen);
void free_minimap (minimap *m);
void MAJMinimap(SDL_Rect posJoueur, minimap * m, SDL_Rect camera, int redimensionnement); // New function
void update_1_(SDL_Rect *prochain , SDL_Rect *principal, SDL_Rect *Pminimap, int distance, int longueur,int longueurM);
void update_2_(SDL_Rect *prochain , SDL_Rect *principal, SDL_Rect *Pminimap, int distance, int longueur,int longueurM);
void mouvement(Personne *p ,Personne *pM ,Personne *pMprochaine, int distance, int longueur,int longueurM , SDL_Surface *masked , int sens);
SDL_Color GetPixel(SDL_Surface *Background, int x, int y);
int collisionPP( Personne p, SDL_Surface * Masque);
#endif