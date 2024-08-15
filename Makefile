CC = clang

CFLAGS = -Wall -Wextra -O2

LIBS = -lprocstat

SRC_DIR = ./src
BIN_DIR = ./bin

SRC = $(SRC_DIR)/pvf.c
TARGET = $(BIN_DIR)/pvf

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
