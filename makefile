# Makefile for SDL 1.2 Game

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 $(shell sdl-config --cflags)
LDFLAGS = $(shell sdl-config --libs) -lSDL_image -lSDL_ttf -lSDL_mixer

# Source and object files
SRCS = main.c menu.c background.c enigme1.c enigme2.c ennemie.c joueur.c minimap.c
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = SDL_Game

# Default target
all: $(TARGET)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Clean up compiled files
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Install dependencies (for Ubuntu/Debian)
deps:
	sudo apt-get update && sudo apt-get install -y \
	libsdl1.2-dev \
	libsdl-image1.2-dev \
	libsdl-ttf2.0-dev \
	libsdl-mixer1.2-dev

# Phony targets
.PHONY: all clean run deps

# Notes for Linux/macOS users:
# 1. Run 'make deps' to install necessary SDL packages (Ubuntu/Debian)
# 2. For other distros, install SDL 1.2 development packages:
#    - Fedora/RHEL: sudo dnf install SDL-devel SDL_image-devel SDL_ttf-devel SDL_mixer-devel
#    - Arch: sudo pacman -S sdl sdl_image sdl_ttf sdl_mixer
# 3. Run 'make' to compile
# 4. Run 'make run' to execute