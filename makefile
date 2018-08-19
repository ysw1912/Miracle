CPPFLAGS = -g -Wall
LDFLAGS =

BASE_DIR = base 
NET_DIR = net 

HEADERS = $(wildcard $(BASE_DIR)/*.h)
HEADERS += $(wildcard $(NET_DIR)/*.h)

SOURCE = $(wildcard $(BASE_DIR)/*.cpp)
SOURCE += $(wildcard $(NET_DIR)/*.cpp)

OBJS = $(SOURCE: .cpp=.o)
TESTS = roundtrip	\
	    netcat	

all: $(TESTS)

$(TESTS): $(HEADERS)

$(TESTS):
	g++ $(CPPFLAGS) -o $@ $(filter %.cpp,$^) $(LDFLAGS)

roundtrip: tests/roundtrip.cpp net/inet_address.cpp net/socket.cpp
roundtrip: CPPFLAGS += -std=c++11
roundtrip: LDFLAGS += -lpthread

netcat: tests/netcat.cpp net/acceptor.cpp net/inet_address.cpp net/socket.cpp net/tcp_stream.cpp
netcat: CPPFLAGS += -std=c++11
netcat: LDFLAGS += -lpthread

clean:
	rm -f $(TESTS)
