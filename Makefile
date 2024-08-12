# Имя исполняемого файла
TARGET = pvf

# Каталоги
SRC_DIR = src
BIN_DIR = bin

# Компилятор и флаги
CC = clang
CFLAGS = -Wall -Wextra -O2

# Список исходных файлов
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Объектные файлы
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BIN_DIR)/%.o)

# Цель по умолчанию
all: $(BIN_DIR) $(BIN_DIR)/$(TARGET)

# Создание каталога bin
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Компиляция
$(BIN_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Создание объектных файлов
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка
clean:
	rm -rf $(BIN_DIR)

# Установка
install: $(BIN_DIR)/$(TARGET)
	install -m 0755 $(BIN_DIR)/$(TARGET) /usr/local/bin

# Деинсталляция
uninstall:
	rm -f /usr/local/bin/$(TARGET)

.PHONY: all clean install uninstall
