#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "background.h"
#include "menu.h" // For screen dimensions

// Initialize background with proper error handling
void initBackground(Background* bg, char* filename, int type) {
    // Load background image with error handling
    bg->img = IMG_Load(filename);
    if (!bg->img) {
        printf("Failed to load background image %s: %s\n", filename, IMG_GetError());
        // Create a default colored background as fallback
        bg->img = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
        SDL_FillRect(bg->img, NULL, SDL_MapRGB(bg->img->format, 0, 0, 100)); // Dark blue fallback
    }
    
    // Initialize camera position
    bg->camera_pos.x = 0;
    bg->camera_pos.y = 0;
    bg->camera_pos.w = SCREEN_WIDTH;
    bg->camera_pos.h = SCREEN_HEIGHT;
    
    // Initialize screen position
    bg->pos_screen.x = 0;
    bg->pos_screen.y = 0;
    bg->pos_screen.w = SCREEN_WIDTH;
    bg->pos_screen.h = SCREEN_HEIGHT;
    
    // Set background type
    bg->type = type;
}

// Display background with camera support
void displayBackground(Background bg, SDL_Surface* screen) {
    if (bg.type == 0) {
        // Fixed background - doesn't scroll
        SDL_BlitSurface(bg.img, NULL, screen, &bg.pos_screen);
    } else {
        // Mobile background - takes camera position into account
        if (bg.camera_pos.x + bg.camera_pos.w > bg.img->w) {
            bg.camera_pos.x = bg.img->w - bg.camera_pos.w;
        }
        if (bg.camera_pos.y + bg.camera_pos.h > bg.img->h) {
            bg.camera_pos.y = bg.img->h - bg.camera_pos.h;
        }
        if (bg.camera_pos.x < 0) bg.camera_pos.x = 0;
        if (bg.camera_pos.y < 0) bg.camera_pos.y = 0;
        
        // Blit visible portion based on camera position
        SDL_BlitSurface(bg.img, &bg.camera_pos, screen, &bg.pos_screen);
    }
}

// Scroll background based on player movement
void scrollBackground(Background* bg, int direction, int speed) {
    if (bg->type == 0) {
        // Fixed background - no scrolling
        return;
    }
    
    // Adjust camera position based on direction
    switch (direction) {
        case 0: // Up
            bg->camera_pos.y -= speed;
            break;
            
        case 1: // Down
            bg->camera_pos.y += speed;
            break;
            
        case 2: // Left
            bg->camera_pos.x -= speed;
            break;
            
        case 3: // Right
            bg->camera_pos.x += speed;
            break;
    }
    
    // Ensure camera stays within background bounds
    if (bg->camera_pos.x < 0) bg->camera_pos.x = 0;
    if (bg->camera_pos.y < 0) bg->camera_pos.y = 0;
    if (bg->camera_pos.x + bg->camera_pos.w > bg->img->w) {
        bg->camera_pos.x = bg->img->w - bg->camera_pos.w;
    }
    if (bg->camera_pos.y + bg->camera_pos.h > bg->img->h) {
        bg->camera_pos.y = bg->img->h - bg->camera_pos.h;
    }
}

// Update camera position based on player position
void updateBackgroundCamera(Background* bg, SDL_Rect playerPos) {
    if (bg->type == 0) return; // Fixed backgrounds don't update
    
    // Center camera on player
    bg->camera_pos.x = playerPos.x - (bg->camera_pos.w / 2) + (playerPos.w / 2);
    bg->camera_pos.y = playerPos.y - (bg->camera_pos.h / 2) + (playerPos.h / 2);
    
    // Ensure camera stays within background bounds
    if (bg->camera_pos.x < 0) bg->camera_pos.x = 0;
    if (bg->camera_pos.y < 0) bg->camera_pos.y = 0;
    if (bg->camera_pos.x + bg->camera_pos.w > bg->img->w) {
        bg->camera_pos.x = bg->img->w - bg->camera_pos.w;
    }
    if (bg->camera_pos.y + bg->camera_pos.h > bg->img->h) {
        bg->camera_pos.y = bg->img->h - bg->camera_pos.h;
    }
}

// Initialize game time tracking
void initGameTime(GameTime* gameTime, char* fontPath, int fontSize) {
    // Record start time
    gameTime->startTime = time(NULL);
    
    // Load font
    gameTime->font = TTF_OpenFont(fontPath, fontSize);
    if (!gameTime->font) {
        printf("Failed to load font %s: %s\n", fontPath, TTF_GetError());
        // We'll continue without the font - text won't be displayed
    }
    
    gameTime->timeText = NULL; // Will be created on update
    
    // Position time display at top right
    gameTime->timePos.x = SCREEN_WIDTH - 150;
    gameTime->timePos.y = 10;
}

// Update and display game time
void updateGameTime(GameTime* gameTime, SDL_Surface* screen) {
    if (!gameTime->font) {
        return; // Can't display without font
    }
    
    // Calculate elapsed time
    time_t now = time(NULL);
    int elapsed = (int) difftime(now, gameTime->startTime);
    
    // Format time string
    int hours = elapsed / 3600;
    int mins = (elapsed % 3600) / 60;
    int secs = elapsed % 60;
    char timeString[20];
    sprintf(timeString, "%02d:%02d:%02d", hours, mins, secs);
    
    // Create text surface
    SDL_Color white = {255, 255, 255};
    if (gameTime->timeText) {
        SDL_FreeSurface(gameTime->timeText);
    }
    gameTime->timeText = TTF_RenderText_Solid(gameTime->font, timeString, white);
    
    if (gameTime->timeText) {
        // Display time
        SDL_BlitSurface(gameTime->timeText, NULL, screen, &gameTime->timePos);
    }
}

// Free background resources
void freeBackground(Background* bg) {
    if (bg->img) {
        SDL_FreeSurface(bg->img);
        bg->img = NULL;
    }
}

// Free game time resources
void freeGameTime(GameTime* gameTime) {
    if (gameTime->font) {
        TTF_CloseFont(gameTime->font);
        gameTime->font = NULL;
    }
    
    if (gameTime->timeText) {
        SDL_FreeSurface(gameTime->timeText);
        gameTime->timeText = NULL;
    }
}

// Split screen for multiplayer or special views
void partitionScreen(SDL_Surface* screen, SDL_Surface** surfaces, SDL_Rect* positions, int count) {
    // Fill screen with black to ensure clean partitioning
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    
    // Draw each partition
    for (int i = 0; i < count; i++) {
        if (surfaces[i]) {
            SDL_BlitSurface(surfaces[i], NULL, screen, &positions[i]);
            
            // Draw partition border
            SDL_Rect border = positions[i];
            border.x--; border.y--;
            border.w += 2; border.h += 2;
            
            // Draw border outline only
            SDL_Rect line;
            Uint32 borderColor = SDL_MapRGB(screen->format, 255, 255, 255);
            
            // Top line
            line.x = border.x; line.y = border.y;
            line.w = border.w; line.h = 1;
            SDL_FillRect(screen, &line, borderColor);
            
            // Bottom line
            line.y = border.y + border.h - 1;
            SDL_FillRect(screen, &line, borderColor);
            
            // Left line
            line.y = border.y; line.w = 1; line.h = border.h;
            SDL_FillRect(screen, &line, borderColor);
            
            // Right line
            line.x = border.x + border.w - 1;
            SDL_FillRect(screen, &line, borderColor);
        }
    }
}