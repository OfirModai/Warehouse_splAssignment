CC=g++
CFLAGS=-g -Iinclude
DEPS = $(wildcard include/*.h)
OBJ_DIR = obj
OBJ = $(patsubst src/%.cpp, $(OBJ_DIR)/%.o, $(wildcard src/*.cpp))

all: clean app

app: $(OBJ) 
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ_DIR)/%.o: src/%.cpp $(DEPS)
	@mkdir -p $(OBJ_DIR)  # Create the obj directory if it doesn't exist
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f $(OBJ) app