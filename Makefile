CXX			:= g++
CXX_FLAGS	:= -Wall -Wextra -std=c++17 -ggdb -g -DASIO_STANDALONE

BIN			:= bin
SRC			:= src
INCLUDE		:= -I/usr/include -I/home/manu/Dev/Cpp/jaynebot/include -I/usr/include/lua5.1 -I/home/manu/Dev/Cpp/jaynebot/vendor/flite/include
LIBRARIES	:= -llua5.1 -pthread -lflite_cmu_us_kal -lflite_usenglish -lflite_cmulex -lflite -lm
EXECUTABLE	:= jaynebot

all: $(BIN)/$(EXECUTABLE)

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