#ifndef ENIGME2_H
#define ENIGME2_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <time.h>
#include <stdlib.h>
#include "assets.h"

// Structure to represent a puzzle piece
typedef struct {
    SDL_Surface *image;
    SDL_Rect position;
    SDL_Rect original_position;
    int is_correct;
    int is_dragging;
} PuzzlePiece;

// Structure for the enigme2
typedef struct {
    SDL_Surface *puzzle_image;         // Complete puzzle image for reference
    SDL_Surface *background;           // Background for the enigme
    SDL_Surface *success_message;      // Message for puzzle solved successfully
    SDL_Surface *failure_message;      // Message for puzzle failure
    
    PuzzlePiece pieces[3];            // Array of proposal pieces (3 pieces)
    PuzzlePiece target_piece;          // The piece that needs to be placed
    
    SDL_Rect target_position;          // Position where the correct piece should be placed
    
    int started;                       // Flag to indicate if enigme has started
    int solved;                        // Flag to indicate if enigme is solved
    int time_limit;                    // Time limit in seconds
    Uint32 start_time;                 // Time when enigme started
    Uint32 time_remaining;             // Remaining time
    
    double zoom_factor;                // For rotozoom animation
    int animation_active;              // Flag for active animation
} Enigme2;

// Function to initialize enigme2
void init_enigme2(Enigme2 *e);

// Function to generate random puzzle
void generate_puzzle(Enigme2 *e);

// Function to display the enigme2
void display_enigme2(Enigme2 *e, SDL_Surface *screen);

// Function to handle events for enigme2
int handle_enigme2_events(Enigme2 *e, SDL_Event *event);

// Function to update enigme2 state (time, animation, etc.)
void update_enigme2(Enigme2 *e);

// Function to clean up resources
void free_enigme2(Enigme2 *e);

// Function to start the enigme2
int play_enigme2(SDL_Surface *screen);

#endif // ENIGME2_H