# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17

# Directories
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build

# Output binary name
TARGET := $(BUILD_DIR)/renderer

# Source files and object files
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Default target
all: $(TARGET)

# Link object files to create the final binary
$(TARGET): $(OBJ_FILES)
	@echo "Linking..."
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $@

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Ensure the build directory exists
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Clean target: remove build artifacts
clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all clean
