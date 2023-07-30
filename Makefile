all:
	g++ grubserv.cpp -std=c++1z -g -pthread -o grubserv

clean:
	rm -f server
