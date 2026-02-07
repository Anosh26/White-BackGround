# Makefile for Background Removal Tool

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -I./include -Wall -O2
LDFLAGS = -lm

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Source files
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/process.c \
          $(SRC_DIR)/queue.c \
          $(SRC_DIR)/detect.c \
          $(SRC_DIR)/stb_lib.c

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Target executable
TARGET = whitebg

# Platform-specific settings
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    RM = del /Q
    MKDIR = mkdir
    # Add Windows resource file if it exists
    ifneq (,$(wildcard resources.o))
        OBJECTS += resources.o
    endif
else
    RM = rm -f
    MKDIR = mkdir -p
endif

# Default target
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)

# Link object files into executable
$(TARGET): $(BUILD_DIR) $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build successful! Run with: ./$(TARGET) <image_path>"

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	$(RM) $(BUILD_DIR)/*.o $(TARGET)
	@echo "Clean complete"

# Rebuild from scratch
rebuild: clean all

# Help target
help:
	@echo "Background Removal Tool - Makefile Help"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build the project"
	@echo "  make all      - Same as 'make'"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make rebuild  - Clean and build"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Usage after building:"
	@echo "  ./$(TARGET) <image_path> [threshold] [quality]"
	@echo ""
	@echo "Examples:"
	@echo "  ./$(TARGET) photo.jpg"
	@echo "  ./$(TARGET) photo.jpg 85 95"

.PHONY: all clean rebuild help
