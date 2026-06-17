CC = gcc
CFLAGS = -Wall -Wextra -Isrc

SRC = $(wildcard src/*.c) \
      $(wildcard src/models/*.c) \
      $(wildcard src/services/*.c) \
      $(wildcard src/storage/*.c) \
      $(wildcard src/ui/*.c) \
      $(wildcard src/utils/*.c) \
      $(wildcard src/math/*.c)
OUT = fincalc

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)