CXX			:= g++
CXX_FLAGS	:= -Wall -Wextra -std=c++17 -ggdb -g -DASIO_STANDALONE -O3

BIN			:= bin
SRC			:= src
INCLUDE		:= -I/usr/include -Iinclude -I/usr/include/lua5.1 -Ivendor/flite/include
LIBRARIES	:= -llua5.1 -pthread -lm
EXECUTABLE	:= coachme_bot

all: clean $(BIN)/$(EXECUTABLE)

run: clean all
	@echo "🚀 Executing..."
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	@echo "🚧 Building..."
	$(CXX) $(CXX_FLAGS) $(INCLUDE) $^ -o $@ $(LIBRARIES)

clean:
	@echo "🧹 Clearing..."
	-rm $(BIN)/$(EXECUTABLE)