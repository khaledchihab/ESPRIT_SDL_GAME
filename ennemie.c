#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include "ennemie.h"

// Initialize enemy based on level
void initEnemy(Enemy *enemy, EnemyLevel level, int x, int y) {
    // Initialize basic properties
    enemy->position.x = x;
    enemy->position.y = y;
    enemy->position.w = 64; // Default width
    enemy->position.h = 64; // Default height
    
    enemy->level = level;
    enemy->direction = RIGHT;
    enemy->state = ALIVE;
    
    enemy->frame_current = 0;
    enemy->frame_count = 4;
    enemy->frame_delay = 5;
    enemy->frame_timer = 0;
    
    enemy->current_patrol_point = 0;
    enemy->es_active = 0;
    
    // Set level-specific properties
    if (level == LEVEL1) {
        enemy->sprite = IMG_Load("enemy1.png");
        enemy->move_speed = 2;
        enemy->health = 50;
        enemy->max_health = 50;
        enemy->damage = 5;
        enemy->detect_range = 150;
        enemy->attack_range = 50;
        enemy->es_sprite = IMG_Load("enemy1_es.png");
    } else { // LEVEL2
        enemy->sprite = IMG_Load("enemy2.png");
        enemy->move_speed = 3;
        enemy->health = 100;
        enemy->max_health = 100;
        enemy->damage = 10;
        enemy->detect_range = 200;
        enemy->attack_range = 60;
        enemy->es_sprite = IMG_Load("enemy2_es.png");
    }
    
    // Set up animation rectangles - these would be adjusted based on your sprite sheet
    for (int state = 0; state < 3; state++) {
        for (int dir = 0; dir < 4; dir++) {
            enemy->animation[state][dir].x = dir * 64;
            enemy->animation[state][dir].y = state * 64;
            enemy->animation[state][dir].w = 64;
            enemy->animation[state][dir].h = 64;
        }
    }
    
    // Set up patrol points for random movement
    enemy->patrol_points[0].x = x;
    enemy->patrol_points[0].y = y;
    enemy->patrol_points[1].x = x + 200; // Random endpoint
    enemy->patrol_points[1].y = y;
    
    // Initialize ES position
    enemy->es_position.x = enemy->position.x;
    enemy->es_position.y = enemy->position.y - 50; // Position ES above enemy
    enemy->es_position.w = 32; // ES width
    enemy->es_position.h = 32; // ES height
}

// Display enemy and its ES if active
void displayEnemy(SDL_Surface *screen, Enemy *enemy) {
    // Only display if not neutralized
    if (enemy->state != NEUTRALIZED) {
        // Determine which part of the sprite sheet to use based on direction and state
        SDL_Rect src_rect = enemy->animation[enemy->state][enemy->direction];
        src_rect.x = src_rect.x + (enemy->frame_current * src_rect.w);
        
        // Blit enemy sprite
        SDL_BlitSurface(enemy->sprite, &src_rect, screen, &enemy->position);
        
        // Display ES if active
        if (enemy->es_active) {
            SDL_BlitSurface(enemy->es_sprite, NULL, screen, &enemy->es_position);
        }
        
        // Draw health bar
        SDL_Rect health_bg = {enemy->position.x, enemy->position.y - 10, enemy->position.w, 5};
        SDL_Rect health_fg = {enemy->position.x, enemy->position.y - 10, (enemy->health * enemy->position.w) / enemy->max_health, 5};
        
        SDL_FillRect(screen, &health_bg, SDL_MapRGB(screen->format, 255, 0, 0)); // Red bg
        SDL_FillRect(screen, &health_fg, SDL_MapRGB(screen->format, 0, 255, 0)); // Green fg
    }
}

// Free enemy resources
void freeEnemy(Enemy *enemy) {
    if (enemy->sprite) {
        SDL_FreeSurface(enemy->sprite);
        enemy->sprite = NULL;
    }
    
    if (enemy->es_sprite) {
        SDL_FreeSurface(enemy->es_sprite);
        enemy->es_sprite = NULL;
    }
}

// Animate enemy by cycling through frames
void animateEnemy(Enemy *enemy) {
    if (enemy->state != NEUTRALIZED) {
        enemy->frame_timer++;
        
        if (enemy->frame_timer > enemy->frame_delay) {
            enemy->frame_current = (enemy->frame_current + 1) % enemy->frame_count;
            enemy->frame_timer = 0;
        }
    }
}

// Move enemy randomly between patrol points
void moveEnemy(Enemy *enemy, SDL_Surface *mask) {
    if (enemy->state == NEUTRALIZED) {
        return; // Don't move if neutralized
    }
    
    // Get target point
    SDL_Rect target = enemy->patrol_points[enemy->current_patrol_point];
    
    // Calculate direction to move
    int dx = 0;
    int dy = 0;
    
    if (enemy->position.x < target.x) {
        dx = enemy->move_speed;
        enemy->direction = RIGHT;
    } else if (enemy->position.x > target.x) {
        dx = -enemy->move_speed;
        enemy->direction = LEFT;
    }
    
    if (enemy->position.y < target.y) {
        dy = enemy->move_speed;
        enemy->direction = DOWN;
    } else if (enemy->position.y > target.y) {
        dy = -enemy->move_speed;
        enemy->direction = UP;
    }
    
    // Check if we reached the target point
    if (abs(enemy->position.x - target.x) < enemy->move_speed &&
        abs(enemy->position.y - target.y) < enemy->move_speed) {
        // Switch to next patrol point
        enemy->current_patrol_point = (enemy->current_patrol_point + 1) % 2;
    }
    
    // Apply movement with proper collision check
    int newX = enemy->position.x + dx;
    int newY = enemy->position.y + dy;
    
    // Only check collision if mask is provided
    if (mask != NULL) {
        // Check boundaries first
        if (newX >= 0 && newX < mask->w && newY >= 0 && newY < mask->h) {
            Uint32 pixel;
            int bpp = mask->format->BytesPerPixel;
            Uint8 *p = (Uint8 *)mask->pixels + newY * mask->pitch + newX * bpp;
            pixel = *(Uint32 *)p;
            
            // If pixel is not black (collision), don't move
            if (pixel != 0) {
                enemy->position.x = newX;
                enemy->position.y = newY;
            }
        }
    } else {
        // No collision mask provided, move freely
        enemy->position.x = newX;
        enemy->position.y = newY;
    }
    
    // Update ES position if active
    if (enemy->es_active) {
        enemy->es_position.x = enemy->position.x;
        enemy->es_position.y = enemy->position.y - 50;
    }
}

// Move enemy with AI towards player
void moveEnemyAI(Enemy *enemy, SDL_Rect player_pos, SDL_Surface *mask) {
    if (enemy->state == NEUTRALIZED) {
        return; // Don't move if neutralized
    }
    
    // Calculate distance to player
    int dx = player_pos.x - enemy->position.x;
    int dy = player_pos.y - enemy->position.y;
    float distance = sqrt(dx*dx + dy*dy);
    
    // If player is within detection range, move toward them
    if (distance < enemy->detect_range) {
        // Normalize direction
        float vx = dx / distance;
        float vy = dy / distance;
        
        // Move in that direction
        int moveX = vx * enemy->move_speed;
        int moveY = vy * enemy->move_speed;
        
        // Set direction based on movement
        if (abs(moveX) > abs(moveY)) {
            enemy->direction = (moveX > 0) ? RIGHT : LEFT;
        } else {
            enemy->direction = (moveY > 0) ? DOWN : UP;
        }
        
        // Apply movement if collision check passes
        // Check for collision at the new position
        int newX = enemy->position.x + moveX;
        int newY = enemy->position.y + moveY;
        
        // Check for collision only if mask is provided
        if (mask != NULL) {
            if (newX >= 0 && newX < mask->w && newY >= 0 && newY < mask->h) {
                Uint32 pixel;
                int bpp = mask->format->BytesPerPixel;
                Uint8 *p = (Uint8 *)mask->pixels + newY * mask->pitch + newX * bpp;
                pixel = *(Uint32 *)p;
                
                // If pixel is not black (collision), don't move
                if (pixel != 0) {
                    enemy->position.x = newX;
                    enemy->position.y = newY;
                }
            }
        } else {
            // No collision mask provided, move freely
            enemy->position.x = newX;
            enemy->position.y = newY;
        }
        
        // Update ES position
        if (enemy->es_active) {
            enemy->es_position.x = enemy->position.x;
            enemy->es_position.y = enemy->position.y - 50;
        }
        
        // If close enough to player, activate ES
        if (distance < enemy->attack_range) {
            activateES(enemy);
        }
    } else {
        // Default to random patrol movement if player is not in range
        moveEnemy(enemy, mask);
    }
}

// Update enemy state based on health
void updateEnemyState(Enemy *enemy) {
    if (enemy->health <= 0) {
        enemy->state = NEUTRALIZED;
    } else if (enemy->health < enemy->max_health / 2) {
        enemy->state = WOUNDED;
    } else {
        enemy->state = ALIVE;
    }
}

// Check for collision with player
int checkCollisionWithPlayer(Enemy *enemy, SDL_Rect player_pos) {
    if (enemy->state == NEUTRALIZED) {
        return 0; // No collision with neutralized enemies
    }
    
    // Simple bounding box collision
    if (enemy->position.x + enemy->position.w > player_pos.x &&
        enemy->position.x < player_pos.x + player_pos.w &&
        enemy->position.y + enemy->position.h > player_pos.y &&
        enemy->position.y < player_pos.y + player_pos.h) {
        return 1; // Collision detected with enemy
    }
    
    // Check for ES collision if active
    if (enemy->es_active) {
        if (enemy->es_position.x + enemy->es_position.w > player_pos.x &&
            enemy->es_position.x < player_pos.x + player_pos.w &&
            enemy->es_position.y + enemy->es_position.h > player_pos.y &&
            enemy->es_position.y < player_pos.y + player_pos.h) {
            return 2; // Collision detected with ES
        }
    }
    
    return 0; // No collision
}

// Damage enemy and update state
void damageEnemy(Enemy *enemy, int damage) {
    enemy->health -= damage;
    if (enemy->health < 0) {
        enemy->health = 0;
    }
    updateEnemyState(enemy);
}

// Activate enemy's ES
void activateES(Enemy *enemy) {
    enemy->es_active = 1;
    // You could add a timer here to deactivate ES after a period
}