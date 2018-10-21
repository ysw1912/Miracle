CPPFLAGS = -g -Wall
LDFLAGS = 

HEADERS = $(wildcard *.h)
TESTS = tests/event_loop_test	\
		tests/event_loop_thread_test	\
		tests/acceptor_test		\

all: $(TESTS)

$(TESTS): $(HEADERS)
$(TESTS):
	g++ $(CPPFLAGS) $(filter %.cpp,$^) -o $@ $(LDFLAGS)

tests/event_loop_test: tests/event_loop_test.cpp ../base/timestamp.cpp ../base/thread.cpp channel.cpp event_loop.cpp poll.cpp timer_queue.cpp
tests/event_loop_test: CPPFLAGS += -std=c++11
tests/event_loop_test: LDFLAGS += -lpthread

tests/event_loop_thread_test: tests/event_loop_thread_test.cpp ../base/timestamp.cpp ../base/thread.cpp channel.cpp event_loop.cpp event_loop_thread.cpp poll.cpp timer_queue.cpp
tests/event_loop_thread_test: CPPFLAGS += -std=c++11
tests/event_loop_thread_test: LDFLAGS += -lpthread

tests/acceptor_test: tests/acceptor_test.cpp ../base/timestamp.cpp ../base/thread.cpp acceptor.cpp channel.cpp event_loop.cpp inet_address.cpp poll.cpp socket.cpp timer_queue.cpp
tests/acceptor_test: CPPFLAGS += -std=c++11
tests/acceptor_test: LDFLAGS += -lpthread

clean:
	rm -f $(TESTS)
