CC=g++
CFLAGS=-g -Wall -Weffc++ -std=c++11 -Iinclude
DEPS = $(wildcard include/*.h)
OBJ_DIR = bin
OBJ = $(patsubst src/%.cpp, $(OBJ_DIR)/%.o, $(wildcard src/*.cpp))
EXECUTABLE = $(OBJ_DIR)/warehouse

all: clean $(EXECUTABLE)

$(EXECUTABLE): $(OBJ) 
	@mkdir -p $(OBJ_DIR)  # Create the bin directory if it doesn't exist
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ_DIR)/%.o: src/%.cpp $(DEPS)
	@mkdir -p $(OBJ_DIR)  # Create the bin directory if it doesn't exist
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f $(OBJ) $(EXECUTABLE)