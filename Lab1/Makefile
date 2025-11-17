CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = index
SRC = index.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Run with default vanban.txt
run:
	./$(TARGET) vanban.txt stopw.txt

# Run with alice30.txt
alice:
	./$(TARGET) alice30.txt stopw.txt

clean:
	rm -f $(TARGET)
