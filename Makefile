CXX=g++
CXXFLAGS += -g -Wall -Wextra -pthread
CPPFLAGS += -isystem src -std=c++11


%.o: %.cpp 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

page_walk: page_walk.o
	$(CXX) -o $@ $^

clean:
	rm -f *~ *.o
	rm -f page_walk
