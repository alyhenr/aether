# --- Compiler and Flags ---
CC = gcc
# -g: Includes debug symbols (crucial for gdb/strace)
# -I./include: Tells the compiler where to find your .h files
# -Wall -Wextra -pedantic: Turns on almost all warnings
# -Werror: Treats all warnings as fatal errors (Wizard mode)
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11 -g -I./include
LDFLAGS = 

# --- Directories ---
SRC_DIR = src
OBJ_DIR = build
INC_DIR = include

# --- Files ---
# Automatically find all .c files in the src/ directory
SRCS = $(wildcard $(SRC_DIR)/*.c)
# Translate the .c filenames into .o filenames in the build/ directory
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# The final executable name
TARGET = $(OBJ_DIR)/aether

# --- Phony Targets ---
# These aren't actual files, just commands
.PHONY: all clean run

# Default target when you just type 'make'
all: $(TARGET)

# --- Build Rules ---

# Rule to link the final executable
$(TARGET): $(OBJS)
	@echo "Linking $@"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile .c files into .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Create the build directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# --- Utility Rules ---

# Clean up compiled files
clean:
	@echo "Cleaning up..."
	rm -rf $(OBJ_DIR)

# Build and immediately run the daemon (Requires sudo for namespaces/cgroups later)
run: all
	sudo ./$(TARGET)