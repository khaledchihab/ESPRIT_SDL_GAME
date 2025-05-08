#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <time.h>
#include <string.h>
#include "enigme1.h"
#include "menu.h" // For screen dimensions
#include "assets.h" // For asset loading functions

// Initialize the puzzle
void init_enigme(enigme* en, SDL_Color Color) {
    // Initialize random number generation
    srand(time(NULL));
      // Load background for the enigma
    en->image_enigme = load_asset_image(ENIGME1_PATH "enigme_bg.png");
    if (!en->image_enigme) {
        printf("Failed to load enigme background: %s\n", IMG_GetError());
        // Create fallback background
        en->image_enigme = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
        SDL_FillRect(en->image_enigme, NULL, SDL_MapRGB(en->image_enigme->format, 0, 50, 100)); // Dark blue fallback
    }
    
    // Position the enigma background to fill the screen
    en->position_enigme.x = 0;
    en->position_enigme.y = 0;
      // Load feedback images for correct/wrong answers
    en->correct = load_asset_image(ENIGME1_PATH "correct.png");
    en->wrong = load_asset_image(ENIGME1_PATH "wrong.png");
    
    if (!en->correct) {
        printf("Failed to load correct image: %s\n", IMG_GetError());
        en->correct = SDL_CreateRGBSurface(SDL_SWSURFACE, 100, 100, 32, 0, 0, 0, 0);
        SDL_FillRect(en->correct, NULL, SDL_MapRGB(en->correct->format, 0, 255, 0)); // Green fallback
    }
    
    if (!en->wrong) {
        printf("Failed to load wrong image: %s\n", IMG_GetError());
        en->wrong = SDL_CreateRGBSurface(SDL_SWSURFACE, 100, 100, 32, 0, 0, 0, 0);
        SDL_FillRect(en->wrong, NULL, SDL_MapRGB(en->wrong->format, 255, 0, 0)); // Red fallback
    }
    
    // Position the feedback images
    en->position_correct.x = (SCREEN_WIDTH - en->correct->w) / 2;
    en->position_correct.y = (SCREEN_HEIGHT - en->correct->h) / 2;
    
    en->position_wrong.x = (SCREEN_WIDTH - en->wrong->w) / 2;
    en->position_wrong.y = (SCREEN_HEIGHT - en->wrong->h) / 2;
      // Initialize fonts
    en->police = load_asset_font(MENU_FONT_PATH, 30);
    en->police_q = load_asset_font(MENU_FONT_PATH, 40);
    
    if (!en->police || !en->police_q) {
        printf("Failed to load font: %s\n", TTF_GetError());
        // Continue without font - text won't be displayed properly
    }
    
    // Initialize text surfaces to NULL
    en->question = NULL;
    en->reponse_1 = NULL;
    en->reponse_2 = NULL;
    en->reponse_3 = NULL;
    
    // Position the text elements
    en->pos_question.x = SCREEN_WIDTH / 2 - 250;
    en->pos_question.y = 150;
    
    en->pos_reponse_1.x = 200;
    en->pos_reponse_1.y = 300;
    
    en->pos_reponse_2.x = 200;
    en->pos_reponse_2.y = 400;
    
    en->pos_reponse_3.x = 200;
    en->pos_reponse_3.y = 500;
    
    // Generate a random question
    alea_enigme(en, Color);
}

// Generate a random question for the puzzle
void alea_enigme(enigme* en, SDL_Color Color) {
    // Free any existing text surfaces
    if (en->question) SDL_FreeSurface(en->question);
    if (en->reponse_1) SDL_FreeSurface(en->reponse_1);
    if (en->reponse_2) SDL_FreeSurface(en->reponse_2);
    if (en->reponse_3) SDL_FreeSurface(en->reponse_3);
    
    // Generate random number between 1 and 5 for question selection
    en->alea = rand() % 5 + 1;
    
    // Check if fonts are available
    if (!en->police || !en->police_q) {
        printf("Fonts not available for rendering text\n");
        return;
    }
    
    // Create text surfaces based on the selected question
    switch (en->alea) {
        case 1:
            en->question = TTF_RenderText_Blended(en->police_q, "What is the capital of France?", Color);
            en->reponse_1 = TTF_RenderText_Blended(en->police, "1: London", Color);
            en->reponse_2 = TTF_RenderText_Blended(en->police, "2: Paris", Color);
            en->reponse_3 = TTF_RenderText_Blended(en->police, "3: Berlin", Color);
            en->resultat = 2;
            break;
            
        case 2:
            en->question = TTF_RenderText_Blended(en->police_q, "What color do you get by mixing blue and yellow?", Color);
            en->reponse_1 = TTF_RenderText_Blended(en->police, "1: Purple", Color);
            en->reponse_2 = TTF_RenderText_Blended(en->police, "2: Orange", Color);
            en->reponse_3 = TTF_RenderText_Blended(en->police, "3: Green", Color);
            en->resultat = 3;
            break;
            
        case 3:
            en->question = TTF_RenderText_Blended(en->police_q, "How many sides does a hexagon have?", Color);
            en->reponse_1 = TTF_RenderText_Blended(en->police, "1: 6", Color);
            en->reponse_2 = TTF_RenderText_Blended(en->police, "2: 8", Color);
            en->reponse_3 = TTF_RenderText_Blended(en->police, "3: 5", Color);
            en->resultat = 1;
            break;
            
        case 4:
            en->question = TTF_RenderText_Blended(en->police_q, "What year did the first Moon landing occur?", Color);
            en->reponse_1 = TTF_RenderText_Blended(en->police, "1: 1971", Color);
            en->reponse_2 = TTF_RenderText_Blended(en->police, "2: 1969", Color);
            en->reponse_3 = TTF_RenderText_Blended(en->police, "3: 1965", Color);
            en->resultat = 2;
            break;
            
        case 5:
            en->question = TTF_RenderText_Blended(en->police_q, "Which planet is closest to the Sun?", Color);
            en->reponse_1 = TTF_RenderText_Blended(en->police, "1: Venus", Color);
            en->reponse_2 = TTF_RenderText_Blended(en->police, "2: Earth", Color);
            en->reponse_3 = TTF_RenderText_Blended(en->police, "3: Mercury", Color);
            en->resultat = 3;
            break;
    }
    
    // Check if text surfaces were created successfully
    if (!en->question || !en->reponse_1 || !en->reponse_2 || !en->reponse_3) {
        printf("Failed to render text for enigme\n");
    }
}

// Draw the enigma on screen
void blit_enigme(enigme *en, SDL_Surface* screen) {
    // Draw background
    SDL_BlitSurface(en->image_enigme, NULL, screen, &en->position_enigme);
    
    // Draw question and answers if available
    if (en->question) SDL_BlitSurface(en->question, NULL, screen, &en->pos_question);
    if (en->reponse_1) SDL_BlitSurface(en->reponse_1, NULL, screen, &en->pos_reponse_1);
    if (en->reponse_2) SDL_BlitSurface(en->reponse_2, NULL, screen, &en->pos_reponse_2);
    if (en->reponse_3) SDL_BlitSurface(en->reponse_3, NULL, screen, &en->pos_reponse_3);
}

// Free all resources used by the enigma
void free_surface_enigme(enigme *en) {
    // Free text surfaces
    if (en->question) {
        SDL_FreeSurface(en->question);
        en->question = NULL;
    }
    
    if (en->reponse_1) {
        SDL_FreeSurface(en->reponse_1);
        en->reponse_1 = NULL;
    }
    
    if (en->reponse_2) {
        SDL_FreeSurface(en->reponse_2);
        en->reponse_2 = NULL;
    }
    
    if (en->reponse_3) {
        SDL_FreeSurface(en->reponse_3);
        en->reponse_3 = NULL;
    }
    
    // Free feedback images
    if (en->correct) {
        SDL_FreeSurface(en->correct);
        en->correct = NULL;
    }
    
    if (en->wrong) {
        SDL_FreeSurface(en->wrong);
        en->wrong = NULL;
    }
    
    // Free background
    if (en->image_enigme) {
        SDL_FreeSurface(en->image_enigme);
        en->image_enigme = NULL;
    }
    
    // Free fonts
    if (en->police) {
        TTF_CloseFont(en->police);
        en->police = NULL;
    }
    
    if (en->police_q) {
        TTF_CloseFont(en->police_q);
        en->police_q = NULL;
    }
}

// Main quiz function that handles user input and returns success/failure
int quiz1(enigme *en, SDL_Surface* screen) {
    SDL_Event event;
    int running = 1;
    int answer = 0;
    int success = 0;
    
    // Draw the enigma
    blit_enigme(en, screen);
    SDL_Flip(screen);
    
    // Add instructions
    SDL_Surface *instructions;
    SDL_Rect inst_pos;
    SDL_Color white = {255, 255, 255, 255};
    
    if (en->police) {
        instructions = TTF_RenderText_Blended(en->police, "Press 1, 2, or 3 to select your answer", white);
        inst_pos.x = (SCREEN_WIDTH - instructions->w) / 2;
        inst_pos.y = SCREEN_HEIGHT - 100;
        SDL_BlitSurface(instructions, NULL, screen, &inst_pos);
        SDL_FreeSurface(instructions);
    }
    
    SDL_Flip(screen);
    
    // Wait for user input
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
                return 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_1:
                    case SDLK_KP1:
                        answer = 1;
                        running = 0;
                        break;
                        
                    case SDLK_2:
                    case SDLK_KP2:
                        answer = 2;
                        running = 0;
                        break;
                        
                    case SDLK_3:
                    case SDLK_KP3:
                        answer = 3;
                        running = 0;
                        break;
                        
                    case SDLK_ESCAPE:
                        running = 0;
                        return 0;
                }
            }
        }
        
        // Cap frame rate
        SDL_Delay(10);
    }
    
    // Check if answer is correct
    if (answer == en->resultat) {
        // Show correct feedback
        SDL_BlitSurface(en->correct, NULL, screen, &en->position_correct);
        success = 1;
    } else {
        // Show incorrect feedback
        SDL_BlitSurface(en->wrong, NULL, screen, &en->position_wrong);
        success = 0;
    }
    
    SDL_Flip(screen);
    
    // Wait a moment to show feedback
    SDL_Delay(1500);
    
    // Generate a new question for next time
    alea_enigme(en, white);
    
    return success;
}

