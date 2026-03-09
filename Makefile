# Compiler
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Iinclude

# Directories
SRC_DIR = engine/src
BUILD_DIR = engine/build

# Find all .cpp files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Convert src/file.cpp → build/file.o
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Final executable name
TARGET = $(BUILD_DIR)/game

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Rebuild
re: clean all

.PHONY: all clean re
