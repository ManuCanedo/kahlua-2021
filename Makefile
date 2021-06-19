CXX			:= g++
CXX_FLAGS	:= -Wall -Wextra -std=c++17 -g -DASIO_STANDALONE -O2

BIN			:= bin
SRC			:= src
INCLUDE		:= -I/usr/include -Iinclude -I/usr/include/lua5.1
LIBRARIES	:= -llua5.1 -pthread -lm
EXECUTABLE	:= kahlua

PCH_SRC 	:= include/net_interface.h
CONFIG_FILES = config.lua commands.lua

all: clean $(BIN)/$(EXECUTABLE)

run: clean all
	@echo "🚀 Executing..."
	clear
	./$(BIN)/$(EXECUTABLE)

pch:
	$(CXX) $(CXX_FLAGS) $(INCLUDE) $(PCH_SRC)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	@echo "🚧 Building..."
	mkdir -p $(BIN)
	cp -u $(CONFIG_FILES) $(BIN)
	$(CXX) $(CXX_FLAGS) $(INCLUDE) $^ -o $@ $(LIBRARIES)

clean:
	@echo "🧹 Cleaning..."
	-rm -f $(BIN)/$(EXECUTABLE)