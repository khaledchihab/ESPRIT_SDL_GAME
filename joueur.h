#ifndef JOUEUR_H_INCLUDED
#define JOUEUR_H_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

// Player states enum
typedef enum {
    IDLE,
    WALKING,
    RUNNING,
    JUMPING,
    ATTACKING
} PlayerState;

// Player direction
typedef enum {
    RIGHT,
    LEFT
} PlayerDirection;

// Player structure
typedef struct {
    // Character attributes
    char name[20];
    int score;
    int lives;

    // Graphics and animation
    SDL_Surface *sprite;    // Current sprite sheet
    SDL_Rect position;      // Position on screen
    SDL_Rect camera;        // Camera focused on player
    
    // Animation related
    SDL_Rect frames[5][10]; // Animation frames [state][frame]
    int currentFrame;       // Current animation frame
    int frameCount;         // Total frames for current animation
    int frameDelay;         // Delay between frame changes
    int frameTimer;         // Timer for animation
    
    // Movement and state
    int velocity;           // Horizontal velocity
    int jumpVelocity;       // Vertical velocity for jumping
    int onGround;           // Flag if player is on ground
    PlayerState state;      // Current player state
    PlayerDirection direction; // Current facing direction
    
    // Sound effects
    Mix_Chunk *soundJump;
    Mix_Chunk *soundAttack;
    Mix_Chunk *soundHurt;
} Joueur;

// Function prototypes

// 1. Initialize and display player
void initialiser_joueur(Joueur *joueur, char *name, char *spritePath);
void afficher_joueur(Joueur joueur, SDL_Surface *screen);

// 2. Lives/Score management
void update_score(Joueur *joueur, int points);
void update_lives(Joueur *joueur, int change);
void display_stats(Joueur joueur, SDL_Surface *screen, TTF_Font *font);

// 3. Player animation
void animer_joueur(Joueur *joueur);
void set_player_state(Joueur *joueur, PlayerState newState);

// 4. Player movement
void deplacer_joueur(Joueur *joueur, int dx, int dy, SDL_Surface *collision_mask);
void marcher_joueur(Joueur *joueur, PlayerDirection direction, SDL_Surface *collision_mask);
void courir_joueur(Joueur *joueur, PlayerDirection direction, SDL_Surface *collision_mask);
void sauter_joueur(Joueur *joueur);
void attaquer_joueur(Joueur *joueur);

// 5. Second player
Joueur initialiser_joueur2(char *name, char *spritePath);

// 6. Character selection submenu
typedef struct {
    SDL_Surface *menuBg;
    SDL_Surface *characterSprites[4]; // Different character options
    SDL_Surface *clothingOptions[4][3]; // Different clothing per character
    SDL_Rect positions[4]; // Positions for each character
    int selectedCharacter;
    int selectedClothing;
    SDL_Rect controlsInstruction; // Position for displaying control instructions
} CharacterSelectMenu;

void init_character_select(CharacterSelectMenu *menu);
void display_character_select(CharacterSelectMenu menu, SDL_Surface *screen);
Joueur select_character(CharacterSelectMenu menu, int characterIndex, int clothingIndex);
void free_character_select(CharacterSelectMenu *menu);
void handle_character_select_input(CharacterSelectMenu *menu, SDL_Event event, int *isDone);

// Collision detection
int collision_joueur_obstacle(Joueur joueur, SDL_Surface *mask);

// Memory management
void liberer_joueur(Joueur *joueur);

#endif // JOUEUR_H_INCLUDED

