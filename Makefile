CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Iinclude

SRC_DIRS = engine/src server
BUILD_DIR = build

# Find all source files
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

# Convert source files to object files
OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

TARGET = $(BUILD_DIR)/game

all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Compile rule
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

re: clean all

.PHONY: all clean re