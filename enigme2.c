#include "enigme2.h"
#include "assets.h"

#define MAX_ZOOM 1.5
#define MIN_ZOOM 1.0
#define ZOOM_STEP 0.05
#define ANIMATION_DURATION 2000 // 2 seconds

// Initialize enigme2
void init_enigme2(Enigme2 *e) {
    // Initialize random seed
    srand(time(NULL));
    
    // Load background
    e->background = load_asset_image(ENIGME2_PATH "background.png");
    if (!e->background) {
        printf("Failed to load background image: %s\n", IMG_GetError());
    }
    
    // Load messages
    e->success_message = load_asset_image(ENIGME2_PATH "success.png");
    e->failure_message = load_asset_image(ENIGME2_PATH "failure.png");
    
    if (!e->success_message) printf("Failed to load success message: %s\n", IMG_GetError());
    if (!e->failure_message) printf("Failed to load failure message: %s\n", IMG_GetError());
    
    // Initialize puzzle pieces
    for (int i = 0; i < 3; i++) {
        e->pieces[i].image = NULL;
        e->pieces[i].is_correct = 0;
        e->pieces[i].is_dragging = 0;
    }
    
    e->target_piece.image = NULL;
    e->target_piece.is_dragging = 0;
    
    // Initialize other properties
    e->puzzle_image = NULL;
    e->started = 0;
    e->solved = 0;
    e->time_limit = 60;  // 60 seconds to solve
    e->start_time = 0;
    e->time_remaining = e->time_limit * 1000; // Convert to milliseconds
    
    e->zoom_factor = MIN_ZOOM;
    e->animation_active = 0;
    
    // Generate the puzzle
    generate_puzzle(e);
}

// Generate random puzzle
void generate_puzzle(Enigme2 *e) {
    // Free previous images if they exist
    if (e->puzzle_image) SDL_FreeSurface(e->puzzle_image);
    for (int i = 0; i < 3; i++) {
        if (e->pieces[i].image) SDL_FreeSurface(e->pieces[i].image);
    }
    if (e->target_piece.image) SDL_FreeSurface(e->target_piece.image);    
    // Choose a random puzzle from 3 possibilities
    int puzzle_num = rand() % 3 + 1;
    char puzzle_path[100];
    snprintf(puzzle_path, sizeof(puzzle_path), "%spuzzle%d.png", ENIGME2_PATH, puzzle_num);
    
    e->puzzle_image = load_asset_image(puzzle_path);
    if (!e->puzzle_image) {
        printf("Failed to load puzzle image: %s\n", IMG_GetError());
        return;
    }
    
    // Load the target piece (piece with shadow/missing part)
    char target_path[100];
    snprintf(target_path, sizeof(target_path), "%spuzzle%d_target.png", ENIGME2_PATH, puzzle_num);
    e->target_piece.image = load_asset_image(target_path);
      // Position for the incomplete puzzle (centered at the top)
    e->target_position.x = (SCREEN_WIDTH - e->puzzle_image->w) / 2;
    e->target_position.y = 70; // Adjusted for higher resolution
    e->target_position.w = e->puzzle_image->w;
    e->target_position.h = e->puzzle_image->h;
    
    // Load the 3 proposal pieces, one correct and two incorrect
    int correct_piece = rand() % 3;  // Randomly choose which piece is correct
    
    for (int i = 0; i < 3; i++) {
        char piece_path[100];
        if (i == correct_piece) {            // Load correct piece
            snprintf(piece_path, sizeof(piece_path), "%spuzzle%d_piece_correct.png", ENIGME2_PATH, puzzle_num);
            e->pieces[i].is_correct = 1;
        } else {
            // Load incorrect pieces
            snprintf(piece_path, sizeof(piece_path), "%spuzzle%d_piece_wrong%d.png", ENIGME2_PATH, puzzle_num, i + (i >= correct_piece ? 1 : 0));
            e->pieces[i].is_correct = 0;
        }
        
        e->pieces[i].image = load_asset_image(piece_path);
        if (!e->pieces[i].image) {
            printf("Failed to load piece image %d: %s\n", i, IMG_GetError());
        }
          // Position the pieces at the bottom of the screen, equally spaced
        e->pieces[i].position.x = (SCREEN_WIDTH / 4) * (i + 1) - (e->pieces[i].image ? e->pieces[i].image->w / 2 : 0);
        e->pieces[i].position.y = SCREEN_HEIGHT - 150;
        e->pieces[i].position.w = e->pieces[i].image->w;
        e->pieces[i].position.h = e->pieces[i].image->h;
        
        // Save original position for reset
        e->pieces[i].original_position = e->pieces[i].position;
    }
    
    // Position the target piece
    e->target_piece.position.x = e->target_position.x;
    e->target_piece.position.y = e->target_position.y;
    e->target_piece.position.w = e->target_piece.image->w;
    e->target_piece.position.h = e->target_piece.image->h;
    
    // Reset timer
    e->start_time = SDL_GetTicks();
    e->time_remaining = e->time_limit * 1000;
    
    e->started = 1;
    e->solved = 0;
    e->animation_active = 0;
}

// Display the enigme2
void display_enigme2(Enigme2 *e, SDL_Surface *screen) {
    // Draw background
    if (e->background) {
        SDL_BlitSurface(e->background, NULL, screen, NULL);
    } else {
        // Fill with a color if background image not available
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 200, 200, 255));
    }
    
    // Draw target puzzle with missing piece
    SDL_BlitSurface(e->target_piece.image, NULL, screen, &e->target_piece.position);
    
    // Draw proposal pieces
    for (int i = 0; i < 3; i++) {
        if (e->pieces[i].image) {
            SDL_BlitSurface(e->pieces[i].image, NULL, screen, &e->pieces[i].position);
        }
    }
    
    // Draw timer as a progress bar
    Uint32 elapsed = SDL_GetTicks() - e->start_time;
    Uint32 remaining = e->time_limit * 1000 - elapsed;
    if (remaining > e->time_limit * 1000) remaining = 0;  // Handle overflow
    
    int bar_width = 400;
    int bar_height = 20;
    int x = (screen->w - bar_width) / 2;
    int y = 20;
    
    // Draw bar background
    SDL_Rect bar_bg = {x, y, bar_width, bar_height};
    SDL_FillRect(screen, &bar_bg, SDL_MapRGB(screen->format, 100, 100, 100));
    
    // Draw remaining time bar    float percentage = (float)remaining / (e->time_limit * 1000);
    SDL_Rect bar = {x, y, (int)(bar_width * percentage), bar_height};
    
    // Change bar color based on remaining time
    Uint32 bar_color;
    if (percentage > 0.6)
        bar_color = SDL_MapRGB(screen->format, 0, 255, 0);  // Green
    else if (percentage > 0.3)
        bar_color = SDL_MapRGB(screen->format, 255, 255, 0);  // Yellow
    else
        bar_color = SDL_MapRGB(screen->format, 255, 0, 0);  // Red
    
    SDL_FillRect(screen, &bar, bar_color);
    
    // Display success/failure message if solved or time's up
    if (e->solved || remaining <= 0) {
        SDL_Surface *message = e->solved ? e->success_message : e->failure_message;
        if (message && e->animation_active) {
            // Apply simple animation effect without using SDL_rotozoom
            SDL_Rect src_rect = {0, 0, message->w, message->h};
            SDL_Rect pos;
            
            // Calculate position to center the image on screen
            pos.x = (screen->w - (int)(message->w * e->zoom_factor)) / 2;
            pos.y = (screen->h - (int)(message->h * e->zoom_factor)) / 2;
            
            // For animation effect, we'll just vary the position and size slightly
            if (e->zoom_factor > 1.0) {
                // Adjust width and height based on zoom factor
                pos.w = (int)(message->w * e->zoom_factor);
                pos.h = (int)(message->h * e->zoom_factor);
                
                // Standard SDL blit - no zoom, but centered with animation
                SDL_BlitSurface(message, NULL, screen, &pos);
            } else {
                // No zoom, just center the image
                pos.x = (screen->w - message->w) / 2;
                pos.y = (screen->h - message->h) / 2;
                SDL_BlitSurface(message, NULL, screen, &pos);
            }
        }
    }
}

// Handle events for enigme2
int handle_enigme2_events(Enigme2 *e, SDL_Event *event) {
    if (!e->started || e->animation_active) return 0;
    
    switch (event->type) {
        case SDL_MOUSEBUTTONDOWN:
            if (event->button.button == SDL_BUTTON_LEFT) {
                int x = event->button.x;
                int y = event->button.y;
                
                // Check if any piece was clicked
                for (int i = 0; i < 3; i++) {
                    SDL_Rect pos = e->pieces[i].position;
                    if (x >= pos.x && x < pos.x + pos.w && 
                        y >= pos.y && y < pos.y + pos.h) {
                        e->pieces[i].is_dragging = 1;
                        // Calculate offset from corner for smoother dragging
                        break;
                    }
                }
            }
            break;
            
        case SDL_MOUSEBUTTONUP:
            if (event->button.button == SDL_BUTTON_LEFT) {
                int x = event->button.x;
                int y = event->button.y;
                
                // Check which piece was being dragged
                for (int i = 0; i < 3; i++) {
                    if (e->pieces[i].is_dragging) {
                        e->pieces[i].is_dragging = 0;
                        
                        // Check if piece was dropped in the target area
                        SDL_Rect target = e->target_piece.position;
                        SDL_Rect piece = e->pieces[i].position;
                        
                        // Check overlap with the target position
                        if (x >= target.x && x < target.x + target.w && 
                            y >= target.y && y < target.y + target.h) {
                            
                            // Snap to target position
                            e->pieces[i].position.x = target.x;
                            e->pieces[i].position.y = target.y;
                            
                            // Check if this is the correct piece
                            if (e->pieces[i].is_correct) {
                                e->solved = 1;
                            } else {
                                // Wrong piece, reset its position
                                e->pieces[i].position = e->pieces[i].original_position;
                            }
                            
                            // Start animation sequence
                            e->animation_active = 1;
                            e->zoom_factor = MIN_ZOOM;
                        } else {
                            // Reset position if not dropped in target area
                            e->pieces[i].position = e->pieces[i].original_position;
                        }
                        break;
                    }
                }
            }
            break;
            
        case SDL_MOUSEMOTION:
            int x = event->motion.x;
            int y = event->motion.y;
            
            // Update position of dragged piece
            for (int i = 0; i < 3; i++) {
                if (e->pieces[i].is_dragging) {
                    e->pieces[i].position.x = x - e->pieces[i].position.w / 2;
                    e->pieces[i].position.y = y - e->pieces[i].position.h / 2;
                    break;
                }
            }
            break;
    }
    
    // Check if time's up
    Uint32 elapsed = SDL_GetTicks() - e->start_time;
    if (elapsed >= e->time_limit * 1000 && !e->solved && !e->animation_active) {
        e->animation_active = 1;
        e->zoom_factor = MIN_ZOOM;
    }
    
    return 0;
}

// Update enigme2 state (time, animation, etc.)
void update_enigme2(Enigme2 *e) {
    // Update timer
    if (e->started && !e->solved && !e->animation_active) {
        Uint32 elapsed = SDL_GetTicks() - e->start_time;
        if (elapsed >= e->time_limit * 1000) {
            e->animation_active = 1;
        }
    }
    
    // Update animation
    if (e->animation_active) {
        static Uint32 last_zoom_time = 0;
        Uint32 current_time = SDL_GetTicks();
        
        if (current_time - last_zoom_time > 50) {  // Update zoom every 50ms
            last_zoom_time = current_time;
            
            // Zoom in until max, then zoom out
            static int zoom_direction = 1;  // 1 for in, -1 for out
            
            e->zoom_factor += ZOOM_STEP * zoom_direction;
            
            if (e->zoom_factor >= MAX_ZOOM) {
                zoom_direction = -1;
            } else if (e->zoom_factor <= MIN_ZOOM) {
                zoom_direction = 1;
                
                // If we're back to minimum zoom, end animation after a few cycles
                static int cycles = 0;
                cycles++;
                if (cycles >= 3) {  // 3 zoom cycles
                    e->animation_active = 0;
                    cycles = 0;
                    
                    // Return to main game if puzzle is solved or failed
                    return;
                }
            }
        }
    }
}

// Clean up resources
void free_enigme2(Enigme2 *e) {
    if (e->puzzle_image) SDL_FreeSurface(e->puzzle_image);
    if (e->background) SDL_FreeSurface(e->background);
    if (e->success_message) SDL_FreeSurface(e->success_message);
    if (e->failure_message) SDL_FreeSurface(e->failure_message);
    
    for (int i = 0; i < 3; i++) {
        if (e->pieces[i].image) SDL_FreeSurface(e->pieces[i].image);
    }
    
    if (e->target_piece.image) SDL_FreeSurface(e->target_piece.image);
    
    e->puzzle_image = NULL;
    e->background = NULL;
    e->success_message = NULL;
    e->failure_message = NULL;
    e->target_piece.image = NULL;
    
    for (int i = 0; i < 3; i++) {
        e->pieces[i].image = NULL;
    }
}

// Function to start and play the enigme2
int play_enigme2(SDL_Surface *screen) {
    Enigme2 e;
    init_enigme2(&e);
    
    SDL_Event event;
    int quit = 0;
    int result = 0;  // 0 for failure, 1 for success
    
    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                quit = 1;
            }
            
            handle_enigme2_events(&e, &event);
        }
        
        // Update game state
        update_enigme2(&e);
        
        // Display game
        display_enigme2(&e, screen);
        
        // Flip buffer
        SDL_Flip(screen);
        
        // Delay to control frame rate
        SDL_Delay(16);  // ~60 FPS
        
        // Check if enigme is finished
        if (e.solved && !e.animation_active) {
            result = 1;  // Success
            SDL_Delay(1000);  // Wait a second before quitting
            quit = 1;
        } else if (SDL_GetTicks() - e.start_time >= e.time_limit * 1000 && !e.animation_active) {
            result = 0;  // Failure
            SDL_Delay(1000);  // Wait a second before quitting
            quit = 1;
        }
    }
    
    free_enigme2(&e);
    return result;
}