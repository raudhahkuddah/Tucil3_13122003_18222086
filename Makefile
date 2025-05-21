CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17

BIN_DIR = bin
TARGET = $(BIN_DIR)/main.exe

SRC = src/main.cpp \
      src/class/Board/Board.cpp \
      src/class/Algo/Solver/Solver.cpp \
      src/class/Algo/Heuristics/Heuristics.cpp

# Default rule
all: $(TARGET)

# Create bin directory and build target
$(TARGET): $(SRC)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Clean up
clean:
	if exist $(BIN_DIR) rmdir /s /q $(BIN_DIR)

.PHONY: all clean
