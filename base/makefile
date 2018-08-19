CPPFLAGS = -g -Wall
LDFLAGS = 

all: test

test: test.cpp timestamp.cpp 
	g++ $(CPPFLAGS) $(filter %.cpp,$^) -o $@ $(LDFLAGS)

clean:
	rm -f test 
