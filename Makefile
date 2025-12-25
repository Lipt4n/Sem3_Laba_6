CXX = clang++
CXXFLAGS = -Wall -std=c++20 -I/opt/homebrew/opt/libpq/include -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/opt/libpq/lib -L/opt/homebrew/lib -lpqxx -lpq -g
TARGET = my_app

FILES = main.cpp utils.cpp db_utils.cpp globals.cpp

all:
	$(CXX) $(CXXFLAGS) $(FILES) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)
