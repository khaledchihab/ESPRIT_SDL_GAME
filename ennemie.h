#ifndef ENNEMIE_H
#define ENNEMIE_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// Direction enumeration for enemy movement
typedef enum {
    ENEMY_LEFT,
    ENEMY_RIGHT,
    ENEMY_UP,
    ENEMY_DOWN,
    ENEMY_IDLE
} Direction;

// Enemy state enumeration
typedef enum {
    ALIVE,
    WOUNDED,
    NEUTRALIZED
} EnemyState;

// Enemy level enumeration
typedef enum {
    LEVEL1,
    LEVEL2
} EnemyLevel;

// Enemy structure
typedef struct {
    SDL_Surface *sprite;          // Enemy sprite sheet
    SDL_Rect position;            // Current position
    SDL_Rect animation[3][4];     // Animation frames [state][direction]
    
    Direction direction;          // Current direction
    EnemyState state;             // Current state (alive, wounded, neutralized)
    EnemyLevel level;             // Enemy level
    
    int health;                   // Current health points
    int max_health;               // Maximum health points
    int damage;                   // Damage dealt to player
    
    int frame_current;            // Current animation frame
    int frame_count;              // Total frames in animation
    int frame_delay;              // Delay between frame changes
    int frame_timer;              // Timer for animation
    
    int move_speed;               // Movement speed
    int detect_range;             // Detection range for player
    int attack_range;             // Range for attacking player
    
    SDL_Rect patrol_points[2];    // Patrol points for random movement
    int current_patrol_point;     // Current patrol point index
    
    // ES (Special Entity) associated with this enemy
    SDL_Surface *es_sprite;       // ES sprite
    SDL_Rect es_position;         // ES position
    int es_active;                // Is ES active
} Enemy;

// Function declarations
void initEnemy(Enemy *enemy, EnemyLevel level, int x, int y);
void displayEnemy(SDL_Surface *screen, Enemy *enemy);
void freeEnemy(Enemy *enemy);
void animateEnemy(Enemy *enemy);
void moveEnemy(Enemy *enemy, SDL_Surface *mask);
void moveEnemyAI(Enemy *enemy, SDL_Rect player_pos, SDL_Surface *mask);
void updateEnemyState(Enemy *enemy);
int checkCollisionWithPlayer(Enemy *enemy, SDL_Rect player_pos);
void damageEnemy(Enemy *enemy, int damage);
void activateES(Enemy *enemy);

#endif // ENNEMIE_H