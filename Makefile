CC = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS = -lasound -ludev
SRC = src/main.c src/alsa_utils.c src/udev_utils.c
OBJ = $(SRC:.c=.o)
TARGET = build/aconnectd

all: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p build
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build $(OBJ)

