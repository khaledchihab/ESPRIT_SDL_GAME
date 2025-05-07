#ifndef ENIGME1_H_INCLUDED
#define ENIGME1_H_INCLUDED
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <string.h>
#include <math.h>
#include <time.h>

struct enigme
{
	SDL_Surface* image_enigme;
	SDL_Rect position_enigme;
	int resultat;
	SDL_Surface *correct,*wrong;
	SDL_Rect position_correct,position_wrong;

	TTF_Font *police,*police_q;
	SDL_Surface *reponse_1,*reponse_2,*reponse_3,*question;
	SDL_Rect  pos_reponse_1,pos_reponse_2,pos_reponse_3,pos_question;
	int alea;
	
};
typedef struct enigme enigme;

void alea_enigme(enigme* en,SDL_Color Color);
void init_enigme( enigme* en ,SDL_Color Color);
void blit_enigme ( enigme *en, SDL_Surface* screen);
void free_surface_enigme( enigme *en );
int quiz1(enigme *en, SDL_Surface* screen );
#endif // ENIGME_H_INCLUDED
