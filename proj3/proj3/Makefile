CC = gcc
CFLAGS = -Wall -C

TARGET = 20182204.out
SRC = *.c
OBJ = $(SRC:.c=.o)

all : $(TARGET)

$(TARGET) : $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean
