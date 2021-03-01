CXX			:= g++
CXX_FLAGS	:= -Wall -Wextra -std=c++17 -ggdb -g -DASIO_STANDALONE

BIN			:= bin
SRC			:= src
INCLUDE		:= -I/usr/include
LIBRARIES	:= -llua5.1 -pthread
EXECUTABLE	:= jaynebot


all: $(BIN)/$(EXECUTABLE)

run: clean all
	@echo "🚀 Executing..."
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	@echo "🚧 Building..."
	$(CXX) $(CXX_FLAGS) $(INCLUDE) $^ -o $@ $(LIBRARIES)

clean:
	@echo "🧹 Clearing..."
	-rm $(BIN)/*