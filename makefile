# Makefile for SDL 1.2 Menu System

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer

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
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Install SDL 1.2 dependencies (for Ubuntu)
deps:
	sudo apt-get update && sudo apt-get install -y \
	libsdl1.2-dev \
	libsdl-image1.2-dev \
	libsdl-ttf2.0-dev \
	libsdl-mixer1.2-dev

# Clean up compiled files
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean deps run