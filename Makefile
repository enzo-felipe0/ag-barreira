CC = gcc
# CFLAGS = -Wall -pthread #Linux
CFLAGS = -Wall -lpthread  #Windows (MinGW)
SRC = src/main.c
OBJ = main
IPATH = -Isrc

all:
	$(CC) $(CFLAGS) $(IPATH) $(SRC) -o $(OBJ)

clean:
	rm -f $(OBJ)

run: all
	./$(OBJ)
