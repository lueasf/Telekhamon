CC = clang
FLAGS = -w -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm -lsqlite3
RELEASE_FLAGS = -O3
#DEBUG_FLAGS = -Wall -Wextra -pedantic -g -fsanitize=address
EXEC = main
C_FILES = $(shell find . -regex ".*/\(.*\.c\)")
O_FILES = $(C_FILES:.c=.o)



.PHONY: all
all: $(O_FILES)
	$(CC) $^ -o $(EXEC) $(DEBUG_FLAGS) $(RELEASE_FLAGS) $(FLAGS)

.PHONY: run
run : all
	./$(EXEC)

%.o: %.c %h
	$(CC) -c $< -o $@ $(DEBUG_FLAGS) $(RELEASE_FLAGS) $(FLAGS)

.PHONY: clean
clean :
	find . -regex ".*/\(.*\.o\|$(EXEC)\)" -not -path '*/.git/*' | xargs rm
