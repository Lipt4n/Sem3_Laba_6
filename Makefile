CXX = clang++

CXXFLAGS = -std=c++20 \
           -I/opt/homebrew/opt/libpq/include \
  		   -I/opt/homebrew/opt/libpqxx/include \
  		   -I/opt/homebrew/include

LDFLAGS = -L/opt/homebrew/opt/libpq/lib \
		  -L/opt/homebrew/opt/libpqxx/lib \
  		  -L/opt/homebrew/lib

LIBS = -lpqxx -lpq

TARGET = my_app
FILES = main.cpp db_utils.cpp globals.cpp cli_utils.cpp input_utils.cpp

all:
	$(CXX) $(FILES) $(CXXFLAGS) $(LDFLAGS) $(LIBS) -o $(TARGET)
