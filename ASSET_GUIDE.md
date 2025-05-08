# Asset Guide for ESPRIT_SDL_GAME

This document provides guidance on preparing assets for the game.

## File Formats

### Images
- Use PNG format for images with transparency
- Use BMP format if experiencing issues with PNG loading
- Ensure images are saved with proper extensions (.png, .bmp)

### Audio
- For music files: Use WAV PCM format (uncompressed) or MP3
- For sound effects: Use WAV PCM format (16-bit, 44.1kHz)
- SDL_mixer supports various formats but PCM WAV is most reliable

### Fonts
- Use TrueType Font (.ttf) files
- Ensure font files are actual TTF files, not renamed text files

## Asset Directory Structure

The game expects assets in the following structure:
```
assets/
  ├── fonts/
  │   └── font.ttf
  ├── music/
  │   └── drumloop.wav
  ├── sounds/
  │   └── hover.wav
  ├── textures/
  │   ├── background1.png
  │   ├── enemy_sprite.png
  │   ├── menu_background.png
  │   ├── menu_button.png
  │   ├── minibg.png
  │   ├── minijoueur.png
  │   └── player_sprite.png
  ├── enigme1/
  │   ├── correct.png
  │   ├── enigme_bg.png
  │   └── wrong.png
  └── enigme2/
      ├── background.png
      └── (other enigme2 assets)
```

## Troubleshooting

If you encounter asset loading issues:

1. Verify file formats:
   - PNG files should be valid PNG format (not JPG renamed to PNG)
   - WAV files should be standard PCM WAV format
   - TTF files should be valid TrueType fonts

2. Check file permissions:
   - Ensure the game has read access to asset files

3. Check for missing files:
   - All required assets should be in their expected locations

4. Convert problematic files:
   - If PNG files don't load, try converting them to BMP
   - If WAV files don't load, try re-saving them as PCM format
   - For fonts, download a standard TrueType font if needed

## Sample Asset Creation

### Creating a simple placeholder image in GIMP:
1. Open GIMP
2. Create a new image (File > New)
3. Fill with a solid color
4. Export as PNG (File > Export As) using default settings

### Creating a simple sound effect:
1. Use Audacity or another audio editor
2. Create a simple sound
3. Export as WAV (File > Export > Export as WAV)
4. Choose "Signed 16-bit PCM" as the format

### Finding free fonts:
- Google Fonts: https://fonts.google.com/
- Font Squirrel: https://www.fontsquirrel.com/
