# ESPRIT SDL GAME

## Overview
This is an SDL 1.2 game project that implements various game components:
- Player character movement and physics
- Enemy AI with collision detection
- Background with camera support
- Menu system
- Minimap functionality
- Two puzzle/enigma mini-games

## Project Structure
- `assets/` - Contains all game resources organized in subdirectories
  - `enigme1/` - Resources for the first puzzle
  - `enigme2/` - Resources for the second puzzle
  - `fonts/` - Game fonts
  - `music/` - Music files
  - `sounds/` - Sound effects
  - `textures/` - Game textures and sprites
- Source files:
  - `main.c` - Main game loop and state management
  - `assets.c/h` - Asset management utilities
  - `menu.c/h` - Menu system
  - `joueur.c/h` - Player character implementation
  - `ennemie.c/h` - Enemy AI implementation
  - `background.c/h` - Background rendering and camera
  - `minimap.c/h` - Minimap functionality
  - `enigme1.c/h` - First puzzle implementation
  - `enigme2.c/h` - Second puzzle implementation

## Setup Instructions
1. Run `setup_assets.bat` to set up the asset directory structure
2. Make sure you have SDL 1.2 development libraries installed:
   - SDL 1.2
   - SDL_image 1.2
   - SDL_ttf 2.0
   - SDL_mixer 1.2
3. Compile the project using the makefile
4. Run the executable

## Game Controls
- Arrow keys: Move player
- Space: Jump
- Esc: Return to menu

## Puzzles
The game includes two puzzle mini-games:
1. Quiz-style puzzle
2. Drag-and-drop puzzle piece matching game