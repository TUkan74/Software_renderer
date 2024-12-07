# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17
LDFLAGS :=

# Directories
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build

# Files
TARGET := $(BUILD_DIR)/renderer
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJ_FILES)
	@echo "Linking..."
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) $(OBJ_FILES) -o $@

# Build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDE_DIR)/%.hpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Clean target
clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all clean
