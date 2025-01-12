CXX       := g++
CXX_FLAGS := -std=c++17 -ggdb

BIN     := bin
SRC     := .
INCLUDE := include

LIBRARIES   :=
EXECUTABLES := aircraftCaptain passenger dispatcher

all: $(addprefix $(BIN)/, $(EXECUTABLES))

run: clean all
	clear
	@for exec in $(EXECUTABLES); do ./$(BIN)/$$exec; done

$(BIN)/%: $(SRC)/%.c
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $^ -o $@ $(LIBRARIES)

clean:
	-rm -f $(BIN)/*
