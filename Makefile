# umieszczenie pliku wykonywalnego w katalogu /bin
TARGET = bin/airplane_simulation

# Kompilator i flagi kompilacji
CC = g++
CFLAGS = -std=c++17 -ggdb -Iinclude -pthread -Wall

# Katalogi
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# Pliki źródłowe
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Reguła główna
all: $(TARGET)

# Tworzenie pliku wykonywalnego
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Kompilowanie plików obiektowych
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Reguła czyszczenia plików tymczasowych
clean:
	rm -rf $(BUILD_DIR)/{captain.o,common.o,dispatcher.o,main.o,passenger.o,utilities.o}  $(TARGET)

# Reguła uruchamiania programu
run: $(TARGET)
	./$(TARGET)