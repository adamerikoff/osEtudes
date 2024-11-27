# Directories and files
BUILD_DIR := ./build
SRC_DIR := ./src
MAIN_FILE := $(SRC_DIR)/tsunami.f90
MOD_DIFF := $(SRC_DIR)/mod_diff.f90
MOD_INITIAL := $(SRC_DIR)/mod_initial.f90
OUTPUT_FILE := $(BUILD_DIR)/tsunami
CALCULATION_FILE := ./output.txt
OUTPUT_PLOT := ./output.jpg
PLOT_SCRIPT := $(SRC_DIR)/plot.py
MOD_DIR := $(BUILD_DIR)/mod # Directory for module files

# Compiler and flags
FC := gfortran
FFLAGS := -O2 -Wall -J$(MOD_DIR) # -J specifies where to put .mod files

# Build target
build: $(BUILD_DIR) $(MOD_DIR) $(OUTPUT_FILE)

# Ensure build directory and module directory exist before anything else
$(BUILD_DIR) $(MOD_DIR):
	@echo "Creating build directory and module directory..."
	mkdir -p $(BUILD_DIR) $(MOD_DIR)

# Link the modules and main program into an executable
$(OUTPUT_FILE): $(MOD_DIFF) $(MOD_INITIAL) $(MAIN_FILE)
	@echo "Compiling and linking Fortran files..."
	$(FC) $(FFLAGS) $(MOD_DIFF) $(MOD_INITIAL) $(MAIN_FILE) -o $(OUTPUT_FILE)

# Run the compiled program and then run the Python plotting script
run: build $(CALCULATION_FILE)
	@echo "Running the program..."
	$(OUTPUT_FILE) > $(CALCULATION_FILE)
	@echo "Running the Python plotting script..."
	python $(PLOT_SCRIPT)

# Generate output.txt by running the program
$(CALCULATION_FILE): $(OUTPUT_FILE)
	@echo "Generating calculation output..."
	$(OUTPUT_FILE) > $(CALCULATION_FILE)

# Clean the build directory
clean:
	@echo "Cleaning build directory and removing output files..."
	rm -rf $(BUILD_DIR) $(OUTPUT_PLOT) $(CALCULATION_FILE)

# Declare phony targets to avoid conflicts with filenames
.PHONY: build run clean
