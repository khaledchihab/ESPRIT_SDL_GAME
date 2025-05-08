#include "assets.h"
#include <stdio.h>
#include <string.h> // For strstr function

// Helper function to load an image asset
SDL_Surface* load_asset_image(const char* path) {
    SDL_Surface* loaded_image = IMG_Load(path);
    
    if (loaded_image == NULL) {
        printf("Unable to load image %s: %s\n", path, IMG_GetError());
        
        // Create a fallback colored surface based on asset type
        SDL_Surface* fallback = SDL_CreateRGBSurface(SDL_SWSURFACE, 64, 64, 32, 0, 0, 0, 0);
        
        // Set different fallback colors based on asset path for easier debugging
        if (strstr(path, "player")) {
            SDL_FillRect(fallback, NULL, SDL_MapRGB(fallback->format, 0, 255, 0)); // Green for player
        }
        else if (strstr(path, "enemy")) {
            SDL_FillRect(fallback, NULL, SDL_MapRGB(fallback->format, 255, 0, 0)); // Red for enemies
        }
        else if (strstr(path, "background")) {
            // Larger surface for backgrounds
            SDL_FreeSurface(fallback);
            fallback = SDL_CreateRGBSurface(SDL_SWSURFACE, 800, 600, 32, 0, 0, 0, 0);
            SDL_FillRect(fallback, NULL, SDL_MapRGB(fallback->format, 50, 50, 150)); // Blue for backgrounds
        }
        else {
            // Default fallback color
            SDL_FillRect(fallback, NULL, SDL_MapRGB(fallback->format, 255, 0, 255)); // Magenta for others
        }
        
        return fallback;
    }
    
    // Convert surface to optimize it
    SDL_Surface* optimized_image = SDL_DisplayFormatAlpha(loaded_image);
    
    // Free the old surface if a new one was created
    if (optimized_image != NULL) {
        SDL_FreeSurface(loaded_image);
        return optimized_image;
    } else {
        // If optimization failed, return original surface
        return loaded_image;
    }
}

// Helper function to load a font asset
TTF_Font* load_asset_font(const char* path, int size) {
    TTF_Font* font = TTF_OpenFont(path, size);
    
    if (font == NULL) {
        printf("Unable to load font %s: %s\n", path, TTF_GetError());
        
        // Try to load a system font as fallback
        // Common system fonts that might be available
        const char* fallback_fonts[] = {
            "C:\\Windows\\Fonts\\arial.ttf",
            "C:\\Windows\\Fonts\\times.ttf",
            "C:\\Windows\\Fonts\\cour.ttf",
            "/usr/share/fonts/truetype/freefont/FreeSans.ttf", // Linux fallback
            NULL
        };
        
        int i = 0;
        while (fallback_fonts[i] != NULL) {
            font = TTF_OpenFont(fallback_fonts[i], size);
            if (font) {
                printf("Using fallback font: %s\n", fallback_fonts[i]);
                break;
            }
            i++;
        }
    }
    
    return font;
}

// Helper function to load a music asset
Mix_Music* load_asset_music(const char* path) {
    Mix_Music* music = Mix_LoadMUS(path);
    
    if (music == NULL) {
        printf("Unable to load music %s: %s\n", path, Mix_GetError());
        // No fallback for music - game will continue without music
        // A more advanced implementation could generate a simple tone or use a built-in fallback track
    }
    
    return music;
}

// Helper function to load a sound effect asset
Mix_Chunk* load_asset_sound(const char* path) {
    Mix_Chunk* sound = Mix_LoadWAV(path);
    
    if (sound == NULL) {
        printf("Unable to load sound effect %s: %s\n", path, Mix_GetError());
        
        // Create a simple beep sound as fallback
        // This requires more complex audio generation that's beyond this fix
        // For now, we'll return NULL and handle it gracefully in the game code
    }
    
    return sound;
}

// Asset verification function
int verify_assets() {
    int missing_count = 0;
    FILE *file;
    
    // Define critical assets to check
    const char* critical_assets[] = {
        PLAYER_SPRITE_PATH,
        MENU_BACKGROUND_PATH,
        MENU_BUTTON_PATH,
        MENU_FONT_PATH,
        BACKGROUND_PATH,
        BACKGROUND_MUSIC,
        NULL
    };
    
    printf("Verifying critical assets...\n");
    
    // Check each critical asset
    int i = 0;
    while (critical_assets[i] != NULL) {
        file = fopen(critical_assets[i], "rb");
        if (file == NULL) {
            printf("MISSING ASSET: %s\n", critical_assets[i]);
            missing_count++;
        } else {
            fclose(file);
        }
        i++;
    }
    
    if (missing_count > 0) {
        printf("WARNING: %d critical assets are missing. Using fallbacks.\n", missing_count);
    } else {
        printf("All critical assets verified successfully.\n");
    }
    
    return (missing_count == 0);
}
