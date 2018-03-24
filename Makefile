CC 		= gcc
CFLAGS 	= -Wall -c -g
LIBS 	= -lncurses
TARGET 	= build/sweeper
SOURCES = src/main.c src/gameboard.c src/random.c
OBJECTS	= $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p build
	$(CC) $(LIBS) -o $@ $(OBJECTS)
%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean
clean: 
	rm -rf $(OBJECTS) $(TARGET)
