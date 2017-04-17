OBJS=AJSP.o PathConstructor.o PathListener.o PathPrinter.o VectorCollector.o main.o
CXXFLAGS=-O0 -g -Wall -pedantic -std=c++11
TARGET=AJSP

all: $(OBJS)
		$(CXX) -o $(TARGET) $(OBJS)

%.o: %.cpp
		$(CXX) -c $< -o $@ $(CXXFLAGS)


clean:
		rm $(TARGET) $(OBJS)

run:
		./$(TARGET)
