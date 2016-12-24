CXX=g++
CXXFLAGS+=-I../ -std=c++11 -Wall -Wno-unused-result -O3 -MMD -MP -DNDEBUG

SRC=$(shell ls *.cpp)

all: $(TARGET)

$(TARGET): $(SRC:.cpp=.o)
	$(CXX) $(CXXFLAGS) $+ -o $@ $(LDFLAGS)

-include $(SRC:.cpp=.d)

clean:
	$(RM) *.o *.d $(TARGET)

.PHONY: all clean

