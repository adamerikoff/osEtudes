# Directories and files
BUILD_DIR := build
SRC_DIR := src
LUA_SRC_DIR := lib/lua/src
MAIN_FILE := $(SRC_DIR)/main.c
OUTPUT_FILE := $(BUILD_DIR)/program

# Compilation flags
CFLAGS := -Wall -Wextra -std=c2x -O2
LDFLAGS := -lSDL2
INCLUDE_PATHS := -I/opt/homebrew/include -I$(LUA_SRC_DIR)
LIBRARY_PATHS := -L/opt/homebrew/lib

# Lua source files (find all .c files in the Lua source directory)
LUA_SRC_FILES := $(wildcard $(LUA_SRC_DIR)/*.c)

# Object files for Lua and main
OBJECT_FILES := $(BUILD_DIR)/main.o $(addprefix $(BUILD_DIR)/, $(notdir $(LUA_SRC_FILES:.c=.o)))

# Build target
build: $(OUTPUT_FILE)

# Compile all source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	mkdir -p $(BUILD_DIR)
	gcc $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LUA_SRC_DIR)/%.c
	@echo "Compiling $<..."
	mkdir -p $(BUILD_DIR)
	gcc $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Link the object files to create the final executable
$(OUTPUT_FILE): $(OBJECT_FILES)
	@echo "Linking object files..."
	gcc $(CFLAGS) $(INCLUDE_PATHS) $(OBJECT_FILES) $(LIBRARY_PATHS) $(LDFLAGS) -o $@

# Run the program
run: build
	@echo "Running the program..."
	./$(OUTPUT_FILE)

# Clean the build directory
clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)

.PHONY: build run clean
