all: server proxy

server: server.cpp
	g++ -std=c++11 -pthread server.cpp -o server

proxy: proxy.cpp
	g++ -std=c++11 -pthread proxy.cpp -o proxy

clean:
	rm -f *.o server proxy