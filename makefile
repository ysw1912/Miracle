CXXFLAGS = -g -Wall

INC_DIR = net 
HEADERS = $(wildcard $(INC_DIR)/*.h)

SOURCE = $(wildcard $(INC_DIR)/*.cpp)
OBJS = $(SOURCE: .cpp=.o)
TESTS = roundtrip 

all: $(TESTS)

$(TESTS): $(HEADERS)

$(TESTS):
	g++ $(CXXFLAGS) -o $@ $(filter %.cpp,$^) $(LDFLAGS)

roundtrip: tests/roundtrip.cpp net/inet_address.cpp net/socket.cpp
roundtrip: CXXFLAGS += --std=c++11

clean:
	rm -f $(TESTS)
