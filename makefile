CPPFLAGS = -g -Wall
LDFLAGS =

BASE_DIR = base 
NET_DIR = net 

HEADERS = $(wildcard $(BASE_DIR)/*.h)
HEADERS += $(wildcard $(NET_DIR)/*.h)

SOURCE = $(wildcard $(BASE_DIR)/*.cpp)
SOURCE += $(wildcard $(NET_DIR)/*.cpp)

OBJS = $(SOURCE: .cpp=.o)
TESTS = chargen	\
		roundtrip	\
	    netcat	

all: $(TESTS)

$(TESTS): $(HEADERS)

$(TESTS):
	g++ $(CPPFLAGS) -o $@ $(filter %.cpp,$^) $(LDFLAGS)

chargen: tests/chargen.cpp base/timestamp.cpp net/inet_address.cpp net/socket.cpp net/tcp_stream.cpp net/acceptor.cpp  
chargen: CPPFLAGS += -std=c++11
chargen: LDFLAGS += -lpthread

roundtrip: tests/roundtrip.cpp net/inet_address.cpp net/socket.cpp
roundtrip: CPPFLAGS += -std=c++11
roundtrip: LDFLAGS += -lpthread

netcat: tests/netcat.cpp net/inet_address.cpp net/socket.cpp net/tcp_stream.cpp net/acceptor.cpp
netcat: CPPFLAGS += -std=c++11
netcat: LDFLAGS += -lpthread

clean:
	rm -f $(TESTS)
