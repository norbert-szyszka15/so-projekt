CC = g++
CFLAGS = -std=c++17 -ggdb -Iinclude -pthread
LDFLAGS = -pthread
SRC_DIR = src
BIN_DIR = bin

# Nazwy plików źródłowych
COMMON_SRC = $(SRC_DIR)/commons.c
PASSENGER_SRC = passenger.c
CAPTAIN_SRC = aircraftCaptain.c
DISPATCHER_SRC = dispatcher.c

COMMON_OBJ = $(SRC_DIR)/commons.o

# Pliki wynikowe
PASSENGER_BIN = $(BIN_DIR)/passenger
CAPTAIN_BIN = $(BIN_DIR)/aircraftCaptain
DISPATCHER_BIN = $(BIN_DIR)/dispatcher

all: $(PASSENGER_BIN) $(CAPTAIN_BIN) $(DISPATCHER_BIN)

$(COMMON_OBJ): $(COMMON_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(PASSENGER_BIN): $(COMMON_OBJ) $(PASSENGER_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $(PASSENGER_SRC) $(COMMON_OBJ) -o $@

$(CAPTAIN_BIN): $(COMMON_OBJ) $(CAPTAIN_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $(CAPTAIN_SRC) $(COMMON_OBJ) -o $@

$(DISPATCHER_BIN): $(COMMON_OBJ) $(DISPATCHER_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $(DISPATCHER_SRC) $(COMMON_OBJ) -o $@

clean:
	-rm $(SRC_DIR)/commons.o
	-rm $(BIN_DIR)/*
