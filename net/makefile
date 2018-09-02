CPPFLAGS = -g -Wall
LDFLAGS = 

HEADERS = $(wildcard *.h)
TESTS = tests/event_loop_test	\

all: $(TESTS)

$(TESTS): $(HEADERS)
$(TESTS):
	g++ $(CPPFLAGS) $(filter %.cpp,$^) -o $@ $(LDFLAGS)

tests/event_loop_test: tests/event_loop_test.cpp ../base/timestamp.cpp ../base/thread.cpp channel.cpp event_loop.cpp poll.cpp timer_queue.cpp
tests/event_loop_test: CPPFLAGS += -std=c++11
tests/event_loop_test: LDFLAGS += -lpthread

clean:
	rm -f $(TESTS)
