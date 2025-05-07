#include "background.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Initialize background
 */
void initBackground(Background* bg, char* filename, int type)
{
    /* Load the image */
    bg->img = IMG_Load(filename);
    if (!bg->img)
    {
        printf("Unable to load background image %s: %s\n", filename, SDL_GetError());
        exit(1);
    }
    
    /* Set the background type */
    bg->type = type;
    
    /* Initialize camera position at top-left corner */
    bg->camera_pos.x = 0;
    bg->camera_pos.y = 0;
    bg->camera_pos.w = bg->img->w;
    bg->camera_pos.h = bg->img->h;
    
    /* Initialize position on screen at origin */
    bg->pos_screen.x = 0;
    bg->pos_screen.y = 0;
    bg->pos_screen.w = bg->img->w;
    bg->pos_screen.h = bg->img->h;
}

/**
 * Display the background
 */
void displayBackground(Background bg, SDL_Surface* screen)
{
    /* Check if the screen exists */
    if (!screen)
    {
        printf("ERROR: Screen not initialized!\n");
        return;
    }

    /* Display based on background type */
    switch (bg.type)
    {
        case 0: /* Fixed background - simple blit */
            SDL_BlitSurface(bg.img, NULL, screen, &bg.pos_screen);
            break;
        
        case 1: /* Mobile background (with camera) */
            SDL_BlitSurface(bg.img, &bg.camera_pos, screen, &bg.pos_screen);
            break;
        
        case 2: /* Destructible background - same as mobile for now */
            SDL_BlitSurface(bg.img, &bg.camera_pos, screen, &bg.pos_screen);
            break;
            
        default:
            printf("WARNING: Unknown background type: %d\n", bg.type);
            SDL_BlitSurface(bg.img, NULL, screen, &bg.pos_screen);
            break;
    }
}

/**
 * Scroll the background in the specified direction
 */
void scrollBackground(Background* bg, int direction, int speed)
{
    /* Only perform scrolling on mobile or destructible backgrounds */
    if (bg->type == 0)
        return;
    
    /* Screen dimensions needed for boundary checking */
    int screen_w = bg->pos_screen.w;
    int screen_h = bg->pos_screen.h;
    
    /* Image dimensions */
    int img_w = bg->img->w;
    int img_h = bg->img->h;
    
    /* Perform scrolling based on direction */
    switch (direction)
    {
        case 0: /* Scroll up */
            if (bg->camera_pos.y > 0)
                bg->camera_pos.y -= speed;
            break;
            
        case 1: /* Scroll down */
            if (bg->camera_pos.y + screen_h < img_h)
                bg->camera_pos.y += speed;
            break;
            
        case 2: /* Scroll left */
            if (bg->camera_pos.x > 0)
                bg->camera_pos.x -= speed;
            break;
            
        case 3: /* Scroll right */
            if (bg->camera_pos.x + screen_w < img_w)
                bg->camera_pos.x += speed;
            break;
            
        default:
            printf("WARNING: Invalid scrolling direction: %d\n", direction);
            break;
    }
}

/**
 * Free background resources
 */
void freeBackground(Background* bg)
{
    if (bg->img)
    {
        SDL_FreeSurface(bg->img);
        bg->img = NULL;
    }
}

/**
 * Initialize the game time
 */
void initGameTime(GameTime* gameTime, char* fontPath, int fontSize)
{
    /* Record start time */
    gameTime->startTime = time(NULL);
    
    /* Initialize TTF if not already done */
    if (!TTF_WasInit() && TTF_Init() == -1)
    {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        exit(1);
    }
    
    /* Load the font */
    gameTime->font = TTF_OpenFont(fontPath, fontSize);
    if (!gameTime->font)
    {
        printf("TTF_OpenFont failed: %s\n", TTF_GetError());
        exit(1);
    }
    
    /* Initial time display */
    gameTime->timeText = NULL;
    
    /* Position for time display (top-right corner with some padding) */
    gameTime->timePos.x = 10;
    gameTime->timePos.y = 10;
    gameTime->timePos.w = 0; /* Will be set during rendering */
    gameTime->timePos.h = 0; /* Will be set during rendering */
}

/**
 * Update and display the game time
 */
void updateGameTime(GameTime* gameTime, SDL_Surface* screen)
{
    /* Calculate elapsed time */
    time_t currentTime = time(NULL);
    time_t elapsedSeconds = currentTime - gameTime->startTime;
    
    /* Format time as HH:MM:SS */
    int hours = elapsedSeconds / 3600;
    int minutes = (elapsedSeconds % 3600) / 60;
    int seconds = elapsedSeconds % 60;
    
    char timeString[20];
    sprintf(timeString, "Time: %02d:%02d:%02d", hours, minutes, seconds);
    
    /* Text color (white) */
    SDL_Color textColor = {255, 255, 255};
    
    /* Free previous surface if it exists */
    if (gameTime->timeText)
    {
        SDL_FreeSurface(gameTime->timeText);
    }
    
    /* Render the new time text */
    gameTime->timeText = TTF_RenderText_Solid(gameTime->font, timeString, textColor);
    if (!gameTime->timeText)
    {
        printf("TTF_RenderText_Solid failed: %s\n", TTF_GetError());
        return;
    }
    
    /* Update dimensions */
    gameTime->timePos.w = gameTime->timeText->w;
    gameTime->timePos.h = gameTime->timeText->h;
    
    /* Blit the time text to the screen */
    SDL_BlitSurface(gameTime->timeText, NULL, screen, &gameTime->timePos);
}

/**
 * Free game time resources
 */
void freeGameTime(GameTime* gameTime)
{
    if (gameTime->timeText)
    {
        SDL_FreeSurface(gameTime->timeText);
        gameTime->timeText = NULL;
    }
    
    if (gameTime->font)
    {
        TTF_CloseFont(gameTime->font);
        gameTime->font = NULL;
    }
}

/**
 * Split screen into multiple views
 */
void partitionScreen(SDL_Surface* screen, SDL_Surface** surfaces, SDL_Rect* positions, int count)
{
    /* Check all parameters */
    if (!screen || !surfaces || !positions || count <= 0)
    {
        printf("ERROR: Invalid parameters for screen partitioning!\n");
        return;
    }
    
    /* Blit each surface to its designated position */
    for (int i = 0; i < count; i++)
    {
        if (surfaces[i])
        {
            SDL_BlitSurface(surfaces[i], NULL, screen, &positions[i]);
        }
    }
}