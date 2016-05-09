CC=g++

CFLAGS=`pkg-config --cflags --libs opencv`
EXECUTABLE=prog

all:
	$(CC) hough.cpp $(CFLAGS) -o $(EXECUTABLE) 2> err.log	