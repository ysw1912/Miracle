CPPFLAGS = -g -Wall
LDFLAGS = 

HEADERS = $(wildcard *.h)
TESTS = tests/race_condition	\
		tests/self_deadlock

all: $(TESTS)

$(TESTS): $(HEADERS)
$(TESTS):
	g++ $(CPPFLAGS) $(filter %.cpp,$^) -o $@ $(LDFLAGS)

tests/race_condition: tests/race_condition.cpp timestamp.cpp thread.cpp 
tests/race_condition: CPPFLAGS += -std=c++11
tests/race_condition: LDFLAGS += -lpthread

tests/self_deadlock: tests/self_deadlock.cpp timestamp.cpp thread.cpp 
tests/self_deadlock: CPPFLAGS += -std=c++11
tests/self_deadlock: LDFLAGS += -lpthread

clean:
	rm -f $(TESTS)
