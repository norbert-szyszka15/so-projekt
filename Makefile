# umieszczenie plików wykonywalnych w odpowiednich katalogach
TARGET = bin/airplane_simulation
TEST_BIN = $(BUILD_DIR)/test_queue

# Kompilator i flagi kompilacji
CC = g++
CFLAGS = -std=c++17 -ggdb -Iinclude -pthread -Wall

# Katalogi
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = tests

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

# Kompilowanie testów
$(TEST_BIN): $(TEST_DIR)/test_queue.c $(SRC_DIR)/utilities.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Reguła czyszczenia plików tymczasowych
clean:
	rm -rf $(BUILD_DIR)/{captain.o,common.o,dispatcher.o,main.o,passenger.o,utilities.o}  $(TARGET) $(TEST_BIN)

# Reguła uruchamiania programu
run: $(TARGET)
	./$(TARGET)

# Reguła uruchamiania testów
test: $(TEST_BIN)
	./$(TEST_BIN)