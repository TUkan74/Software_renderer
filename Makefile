# Makefile for SoftwareRenderer on Linux

# Variables
BUILD_DIR = build
TARGET = software-renderer
CMAKE = cmake
MAKE = make
CMAKE_GENERATOR = "Unix Makefiles"
CONFIGURATION = Release

# Default target
all: build

# Build target
build:
	@echo "Configuring the project with CMake..."
	@if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi
	@cd $(BUILD_DIR) && $(CMAKE) -G $(CMAKE_GENERATOR) -DCMAKE_BUILD_TYPE=$(CONFIGURATION) ..
	@echo "Building the project..."
	@cd $(BUILD_DIR) && $(MAKE) -j$(nproc)

# Clean target
clean:
	@echo "Cleaning build artifacts..."
	@if [ -d $(BUILD_DIR)/$(CONFIGURATION) ]; then rm -rf $(BUILD_DIR)/$(CONFIGURATION); fi

# Clean all target
clean-all:
	@echo "Removing the entire build directory..."
	@if [ -d $(BUILD_DIR) ]; then rm -rf $(BUILD_DIR); fi

# Rebuild target
rebuild: clean build

# Run the project
run:
	@echo "Running the project..."
	@$(BUILD_DIR)/$(TARGET)

# Help target
help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  build        - Configure and build the project"
	@echo "  clean        - Clean build artifacts"
	@echo "  clean-all    - Remove the entire build directory"
	@echo "  rebuild      - Clean and rebuild the project"
	@echo "  run          - Run the project"
	@echo "  help         - Display this help message"

.PHONY: all build clean clean-all rebuild run help
