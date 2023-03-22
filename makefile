FLAG=-std=c++11 -L /usr/lib/json -lboost_filesystem -lboost_system -lpthread -ljsoncpp

engine: ./http/main.cpp  ./searcher/searcher.cpp
	g++ $^ -o $@ $(FLAG)

clean:
	rm -rf engine