#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "joueur.h"
#include "assets.h" // Include for SCREEN_WIDTH, SCREEN_HEIGHT, and path definitions

// 1. Initialize and display player
void initialiser_joueur(Joueur *joueur, char *name, char *spritePath) {
    // Initialize player attributes
    strncpy(joueur->name, name, 19);
    joueur->name[19] = '\0';
    joueur->score = 0;
    joueur->lives = 3;
    
    // Load player sprite
    joueur->sprite = load_asset_image(spritePath);
    if (!joueur->sprite) {
        fprintf(stderr, "Could not load player sprite: %s\n", IMG_GetError());
        // Create a fallback sprite instead of exiting
        joueur->sprite = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 320, 32, 0, 0, 0, 0); // 10x5 frames
        SDL_FillRect(joueur->sprite, NULL, SDL_MapRGB(joueur->sprite->format, 0, 255, 0)); // Green fallback
        printf("Using fallback player sprite\n");
    }
    
    // Set initial position
    joueur->position.x = 100;
    joueur->position.y = 300;
    joueur->position.w = joueur->sprite->w / 10; // Assuming 10 frames per row
    joueur->position.h = joueur->sprite->h / 5;  // Assuming 5 states (rows)
    
    // Camera setup
    joueur->camera.x = 0;
    joueur->camera.y = 0;
    joueur->camera.w = 800; // Default screen width
    joueur->camera.h = 600; // Default screen height
    
    // Animation setup - setup animation frames based on sprite sheet
    int frameWidth = joueur->sprite->w / 10;
    int frameHeight = joueur->sprite->h / 5;
    
    for (int state = 0; state < 5; state++) {
        for (int frame = 0; frame < 10; frame++) {
            joueur->frames[state][frame].x = frame * frameWidth;
            joueur->frames[state][frame].y = state * frameHeight;
            joueur->frames[state][frame].w = frameWidth;
            joueur->frames[state][frame].h = frameHeight;
        }
    }
    
    // Animation state
    joueur->currentFrame = 0;
    joueur->frameCount = 10;    // Default 10 frames per animation
    joueur->frameDelay = 100;   // Milliseconds between frames
    joueur->frameTimer = SDL_GetTicks();
    
    // Movement and state initialization
    joueur->velocity = 0;
    joueur->jumpVelocity = 0;
    joueur->onGround = 1;       // Start on ground
    joueur->state = IDLE;
    joueur->direction = RIGHT;
    
    // Load sound effects if they exist
    joueur->soundJump = load_asset_sound(SOUND_PATH "jump.wav");
    joueur->soundAttack = load_asset_sound(SOUND_PATH "attack.wav");
    joueur->soundHurt = load_asset_sound(SOUND_PATH "hurt.wav");
    
    // Check if sounds loaded properly
    if (!joueur->soundJump || !joueur->soundAttack || !joueur->soundHurt) {
        fprintf(stderr, "Warning: Could not load sound effects. Game will continue without sound.\n");
    }
}

void afficher_joueur(Joueur joueur, SDL_Surface *screen) {
    // Current frame based on state and animation progression
    SDL_Rect frameRect = joueur.frames[joueur.state][joueur.currentFrame];
    SDL_Rect destRect = joueur.position;
    
    // Handle flipping if player faces left
    if (joueur.direction == LEFT) {
        // For flipping the sprite horizontally, we need to use SDL_gfx library or create a flipped surface
        // For now, we'll just indicate it with a different frame or color
        SDL_BlitSurface(joueur.sprite, &frameRect, screen, &destRect);
    } else {
        // Normal blitting for right direction
        SDL_BlitSurface(joueur.sprite, &frameRect, screen, &destRect);
    }
}

// 2. Lives/Score management
void update_score(Joueur *joueur, int points) {
    joueur->score += points;
    // You might want to add maximum score limit or achievements
}

void update_lives(Joueur *joueur, int change) {
    joueur->lives += change;
    
    // Check if player died
    if (joueur->lives <= 0) {
        joueur->lives = 0;
        // Trigger game over or respawn logic here
    }
    
    // Optional: cap maximum lives
    if (joueur->lives > 5) {
        joueur->lives = 5;
    }
    
    // If taking damage, play hurt sound
    if (change < 0 && joueur->soundHurt) {
        Mix_PlayChannel(-1, joueur->soundHurt, 0);
    }
}

void display_stats(Joueur joueur, SDL_Surface *screen, TTF_Font *font) {
    if (!font) {
        fprintf(stderr, "Font not loaded for stats display.\n");
        return;
    }
    
    // Create text surfaces
    SDL_Color textColor = {255, 255, 255, 255}; // White
    char scoreText[50];
    char livesText[20];
    
    sprintf(scoreText, "Score: %d", joueur.score);
    sprintf(livesText, "Lives: %d", joueur.lives);
    
    SDL_Surface *scoreSurface = TTF_RenderText_Solid(font, scoreText, textColor);
    SDL_Surface *livesSurface = TTF_RenderText_Solid(font, livesText, textColor);
    
    // Positions for text
    SDL_Rect scorePos = {10, 10, 0, 0};
    SDL_Rect livesPos = {10, 40, 0, 0};
    
    // Draw text
    SDL_BlitSurface(scoreSurface, NULL, screen, &scorePos);
    SDL_BlitSurface(livesSurface, NULL, screen, &livesPos);
    
    // Free text surfaces
    SDL_FreeSurface(scoreSurface);
    SDL_FreeSurface(livesSurface);
}

// 3. Player animation
void animer_joueur(Joueur *joueur) {
    // Increment animation timer
    joueur->frameTimer++;
    
    // Update frame if timer exceeds delay
    if (joueur->frameTimer > joueur->frameDelay) {
        joueur->currentFrame = (joueur->currentFrame + 1) % joueur->frameCount;
        joueur->frameTimer = 0;
    }
    
    // Apply gravity for jumping
    if (joueur->state == JUMPING) {
        // Apply gravity to jump velocity
        joueur->jumpVelocity += 1;
        
        // Apply the vertical velocity
        deplacer_joueur(joueur, 0, joueur->jumpVelocity, NULL);
        
        // If on ground after movement, end jump
        if (joueur->onGround) {
            set_player_state(joueur, IDLE);
        }
    }
}

void set_player_state(Joueur *joueur, PlayerState newState) {
    // Only change state if different
    if (joueur->state != newState) {
        joueur->state = newState;
        joueur->currentFrame = 0; // Reset to first frame of new animation
        
        // Adjust frame count and delay based on state
        switch (newState) {
            case IDLE:
                joueur->frameCount = 10;
                joueur->frameDelay = 150; // Slower idle animation
                break;
                
            case WALKING:
                joueur->frameCount = 10;
                joueur->frameDelay = 100;
                break;
                
            case RUNNING:
                joueur->frameCount = 10;
                joueur->frameDelay = 70; // Faster running animation
                break;
                
            case JUMPING:
                joueur->frameCount = 8; // Fewer jump frames typically
                joueur->frameDelay = 80;
                break;
                
            case ATTACKING:
                joueur->frameCount = 6; // Fewer attack frames
                joueur->frameDelay = 60; // Fast attack animation
                break;
        }
    }
}

// 4. Player movement
void deplacer_joueur(Joueur *joueur, int dx, int dy, SDL_Surface *collision_mask) {
    // Calculate new position
    int newX = joueur->position.x + dx;
    int newY = joueur->position.y + dy;
    
    // Check for collisions only if a valid collision mask is provided
    if (collision_mask != NULL) {
        // Boundary checks
        if (newX < 0) newX = 0;
        if (newX + joueur->position.w > collision_mask->w) newX = collision_mask->w - joueur->position.w;
        
        if (newY < 0) newY = 0;
        if (newY + joueur->position.h > collision_mask->h) newY = collision_mask->h - joueur->position.h;
        
        // Check for pixel-perfect collisions if needed
        // This is a simplified collision check - you might need more complex logic
        // Example: check all four corners of the player's bounding box
        int collision = 0;
        
        // Only run this check if we're within the bounds of the collision mask
        if (newX >= 0 && newX < collision_mask->w && 
            newY >= 0 && newY < collision_mask->h) {
            
            // Get pixel at the bottom center of the player (for ground collision)
            Uint32 pixel;
            int bpp = collision_mask->format->BytesPerPixel;
            
            // Bottom center pixel
            int checkX = newX + joueur->position.w / 2;
            int checkY = newY + joueur->position.h - 1;
            
            if (checkX >= 0 && checkX < collision_mask->w && 
                checkY >= 0 && checkY < collision_mask->h) {
                
                Uint8 *p = (Uint8 *)collision_mask->pixels + checkY * collision_mask->pitch + checkX * bpp;
                pixel = *(Uint32 *)p;
                
                // If pixel is not black (collision), handle it
                if (pixel != 0) {
                    // For vertical movement, stop falling and set onGround
                    if (dy > 0) {
                        joueur->onGround = 1;
                        joueur->jumpVelocity = 0;
                        collision = 1;
                    }
                    // For upward movement, stop rising
                    else if (dy < 0) {
                        joueur->jumpVelocity = 0;
                        collision = 1;
                    }
                }
            }
            
            // Check side collisions as well
            // Left side center
            checkX = newX;
            checkY = newY + joueur->position.h / 2;
            
            if (checkX >= 0 && checkX < collision_mask->w && 
                checkY >= 0 && checkY < collision_mask->h) {
                
                Uint8 *p = (Uint8 *)collision_mask->pixels + checkY * collision_mask->pitch + checkX * bpp;
                pixel = *(Uint32 *)p;
                
                // If pixel is not black (collision), don't move horizontally
                if (pixel != 0 && dx < 0) {
                    collision = 1;
                }
            }
            
            // Right side center
            checkX = newX + joueur->position.w - 1;
            checkY = newY + joueur->position.h / 2;
            
            if (checkX >= 0 && checkX < collision_mask->w && 
                checkY >= 0 && checkY < collision_mask->h) {
                
                Uint8 *p = (Uint8 *)collision_mask->pixels + checkY * collision_mask->pitch + checkX * bpp;
                pixel = *(Uint32 *)p;
                
                // If pixel is not black (collision), don't move horizontally
                if (pixel != 0 && dx > 0) {
                    collision = 1;
                }
            }
        }
        
        // Apply movement only if there's no collision
        if (!collision) {
            joueur->position.x = newX;
            joueur->position.y = newY;
            
            // If moving down and no collision, we're not on ground
            if (dy > 0) {
                joueur->onGround = 0;
            }
        } else if (dx != 0) {
            // Allow vertical movement even if horizontal collision
            joueur->position.y = newY;
        } else if (dy != 0) {
            // Allow horizontal movement even if vertical collision
            joueur->position.x = newX;
        }
    } else {
        // No collision mask - move freely but respect screen boundaries
        if (newX >= 0 && newX + joueur->position.w <= SCREEN_WIDTH) {
            joueur->position.x = newX;
        }
        
        if (newY >= 0 && newY + joueur->position.h <= SCREEN_HEIGHT) {
            joueur->position.y = newY;
            // If moving down, assume we're not on ground
            if (dy > 0) joueur->onGround = 0;
        } else if (newY + joueur->position.h > SCREEN_HEIGHT) {
            // Hit bottom of screen, assume on ground
            joueur->position.y = SCREEN_HEIGHT - joueur->position.h;
            joueur->onGround = 1;
            joueur->jumpVelocity = 0;
        }
    }
    
    // Update camera position to follow player
    joueur->camera.x = joueur->position.x - (SCREEN_WIDTH / 2) + (joueur->position.w / 2);
    joueur->camera.y = joueur->position.y - (SCREEN_HEIGHT / 2) + (joueur->position.h / 2);
    
    // Keep camera within bounds
    if (joueur->camera.x < 0) joueur->camera.x = 0;
    if (joueur->camera.y < 0) joueur->camera.y = 0;
    // You would also check against the maximum world dimensions here
}

void marcher_joueur(Joueur *joueur, PlayerDirection direction, SDL_Surface *collision_mask) {
    // Set direction
    joueur->direction = direction;
    
    // Set state
    set_player_state(joueur, WALKING);
    
    // Set velocity based on direction
    int moveSpeed = 3; // Walking speed
    int dx = (direction == RIGHT) ? moveSpeed : -moveSpeed;
    
    // Move player
    deplacer_joueur(joueur, dx, 0, collision_mask);
}

void courir_joueur(Joueur *joueur, PlayerDirection direction, SDL_Surface *collision_mask) {
    // Set direction
    joueur->direction = direction;
    
    // Set state
    set_player_state(joueur, RUNNING);
    
    // Set velocity based on direction
    int moveSpeed = 6; // Running speed (faster than walking)
    int dx = (direction == RIGHT) ? moveSpeed : -moveSpeed;
    
    // Move player
    deplacer_joueur(joueur, dx, 0, collision_mask);
}

void sauter_joueur(Joueur *joueur) {
    // Only allow jumping when on the ground
    if (joueur->onGround) {
        joueur->jumpVelocity = -15; // Initial upward velocity
        joueur->onGround = 0;
        
        // Play jump sound
        if (joueur->soundJump) {
            Mix_PlayChannel(-1, joueur->soundJump, 0);
        }
        
        set_player_state(joueur, JUMPING);
    }
}

void attaquer_joueur(Joueur *joueur) {
    // Set attack state
    set_player_state(joueur, ATTACKING);
    
    // Reset frame to beginning of attack animation
    joueur->currentFrame = 0;
    
    // Play attack sound if available
    if (joueur->soundAttack) {
        Mix_PlayChannel(-1, joueur->soundAttack, 0);
    }
    
    // Attack logic would go here (checking for enemies in range, etc.)
}

// 5. Second player initialization - similar to first player but with different defaults
Joueur initialiser_joueur2(char *name, char *spritePath) {
    Joueur joueur2;
    
    // Initialize with default player function but with different starting position
    initialiser_joueur(&joueur2, name, spritePath);
    
    // Override position for player 2
    joueur2.position.x = 200;
    joueur2.position.y = 300;
    
    return joueur2;
}

// 6. Character selection submenu
void init_character_select(CharacterSelectMenu *menu) {    // Load menu background
    char bgPath[100];
    sprintf(bgPath, "%scharacter_select_bg.png", TEXTURE_PATH);
    menu->menuBg = load_asset_image(bgPath);
    if (!menu->menuBg) {
        fprintf(stderr, "Could not load character select background: %s\n", IMG_GetError());
    }
      // Load character sprite options
    for (int i = 0; i < 4; i++) {
        char path[100];
        sprintf(path, "%scharacter_%d.png", TEXTURE_PATH, i+1);
        menu->characterSprites[i] = load_asset_image(path);
        
        if (!menu->characterSprites[i]) {
            fprintf(stderr, "Could not load character sprite %d: %s\n", i+1, IMG_GetError());
        }        // Load clothing options for each character
        for (int j = 0; j < 3; j++) {
            sprintf(path, "%scharacter_%d_clothing_%d.png", TEXTURE_PATH, i+1, j+1);
            menu->clothingOptions[i][j] = load_asset_image(path);
            
            if (!menu->clothingOptions[i][j]) {
                fprintf(stderr, "Could not load clothing option %d for character %d: %s\n", 
                        j+1, i+1, IMG_GetError());
            }
        }
        
        // Set positions for character display
        menu->positions[i].x = 150 + (i * 200);
        menu->positions[i].y = 250;
        menu->positions[i].w = 100;
        menu->positions[i].h = 150;
    }
    
    // Default selections
    menu->selectedCharacter = 0;
    menu->selectedClothing = 0;
    
    // Controls instruction position
    menu->controlsInstruction.x = 100;
    menu->controlsInstruction.y = 500;
}

void display_character_select(CharacterSelectMenu menu, SDL_Surface *screen) {
    // Display background
    if (menu.menuBg) {
        SDL_BlitSurface(menu.menuBg, NULL, screen, NULL);
    }
    
    // Display all characters
    for (int i = 0; i < 4; i++) {
        if (menu.characterSprites[i]) {
            SDL_Rect destRect = menu.positions[i];
            
            // Highlight selected character
            if (i == menu.selectedCharacter) {
                // Draw selection indicator (e.g., rectangle around character)
                SDL_Rect highlightRect = {
                    destRect.x - 5,
                    destRect.y - 5,
                    destRect.w + 10,
                    destRect.h + 10
                };
                
                Uint32 highlightColor = SDL_MapRGB(screen->format, 255, 215, 0); // Gold color
                SDL_FillRect(screen, &highlightRect, highlightColor);
            }
            
            // Draw character with current clothing
            SDL_BlitSurface(menu.characterSprites[i], NULL, screen, &destRect);
            
            // Draw clothing if available
            if (menu.clothingOptions[i][menu.selectedClothing]) {
                SDL_BlitSurface(menu.clothingOptions[i][menu.selectedClothing], NULL, screen, &destRect);
            }
        }
    }
    
    // Display controls instructions
    // This would need a TTF_Font and rendered text surface
}

Joueur select_character(CharacterSelectMenu menu, int characterIndex, int clothingIndex) {
    Joueur selected;
    char name[20] = "Player";
    
    // Validate indices
    if (characterIndex < 0 || characterIndex > 3) characterIndex = 0;
    if (clothingIndex < 0 || clothingIndex > 2) clothingIndex = 0;    // Load appropriate character sprite with clothing
    char spritePath[100];
    sprintf(spritePath, "%scharacter_%d_clothing_%d_spritesheet.png", 
            TEXTURE_PATH, characterIndex + 1, clothingIndex + 1);
    
    // Initialize player with selected appearance
    initialiser_joueur(&selected, name, spritePath);
    
    return selected;
}

void free_character_select(CharacterSelectMenu *menu) {
    // Free background
    if (menu->menuBg) {
        SDL_FreeSurface(menu->menuBg);
        menu->menuBg = NULL;
    }
    
    // Free character sprites and clothing options
    for (int i = 0; i < 4; i++) {
        if (menu->characterSprites[i]) {
            SDL_FreeSurface(menu->characterSprites[i]);
            menu->characterSprites[i] = NULL;
        }
        
        for (int j = 0; j < 3; j++) {
            if (menu->clothingOptions[i][j]) {
                SDL_FreeSurface(menu->clothingOptions[i][j]);
                menu->clothingOptions[i][j] = NULL;
            }
        }
    }
}

void handle_character_select_input(CharacterSelectMenu *menu, SDL_Event event, int *isDone) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                menu->selectedCharacter = (menu->selectedCharacter + 3) % 4;
                break;
                
            case SDLK_RIGHT:
                menu->selectedCharacter = (menu->selectedCharacter + 1) % 4;
                break;
                
            case SDLK_UP:
                menu->selectedClothing = (menu->selectedClothing + 2) % 3;
                break;
                
            case SDLK_DOWN:
                menu->selectedClothing = (menu->selectedClothing + 1) % 3;
                break;
                
            case SDLK_RETURN:
                *isDone = 1; // Selection confirmed
                break;
                
            case SDLK_ESCAPE:
                *isDone = -1; // Cancel selection
                break;
        }
    }
}

// Collision detection
int collision_joueur_obstacle(Joueur joueur, SDL_Surface *mask) {
    // This function should check if the player collides with obstacles
    // We'll use the same approach as used in the minimap.c file

    // Check multiple points around the player's bounding box
    int collisionPoints[][2] = {
        {0, 0},                    // Top-left
        {joueur.position.w/2, 0},  // Top-center
        {joueur.position.w, 0},    // Top-right
        {0, joueur.position.h/2},  // Middle-left
        {joueur.position.w, joueur.position.h/2}, // Middle-right
        {0, joueur.position.h},    // Bottom-left
        {joueur.position.w/2, joueur.position.h}, // Bottom-center
        {joueur.position.w, joueur.position.h}    // Bottom-right
    };
    
    // Function to get pixel color from minimap.c
    extern SDL_Color GetPixel(SDL_Surface *Background, int x, int y);
    
    // Check collision at each point
    for (int i = 0; i < 8; i++) {
        int x = joueur.position.x + collisionPoints[i][0];
        int y = joueur.position.y + collisionPoints[i][1];
        
        // Make sure we're within the mask bounds
        if (x >= 0 && y >= 0 && x < mask->w && y < mask->h) {
            SDL_Color pixel = GetPixel(mask, x, y);
            
            // Black pixels (RGB 0,0,0) represent obstacles
            if (pixel.r == 0 && pixel.g == 0 && pixel.b == 0) {
                return 1; // Collision detected
            }
        }
    }
    
    return 0; // No collision
}

// Memory management
void liberer_joueur(Joueur *joueur) {
    // Free sprite
    if (joueur->sprite) {
        SDL_FreeSurface(joueur->sprite);
        joueur->sprite = NULL;
    }
    
    // Free sound effects
    if (joueur->soundJump) {
        Mix_FreeChunk(joueur->soundJump);
        joueur->soundJump = NULL;
    }
    
    if (joueur->soundAttack) {
        Mix_FreeChunk(joueur->soundAttack);
        joueur->soundAttack = NULL;
    }
    
    if (joueur->soundHurt) {
        Mix_FreeChunk(joueur->soundHurt);
        joueur->soundHurt = NULL;
    }
}

