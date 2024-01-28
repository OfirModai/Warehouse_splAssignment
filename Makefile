CC=g++
CFLAGS=-g -Iinclude  
DEPS = $(wildcard include/*.h)
OBJ = $(patsubst src/%.cpp, %.o, $(wildcard src/*.cpp))

app: $(OBJ) 
	$(CC) -o $@ $^ $(CFLAGS)
	
%.o: src/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
clean: 
	rm -f $(OBJ) app