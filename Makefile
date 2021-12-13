CXX = g++
CPPFLAGS = -Wall -g -std=c++14

all: central servert servers serverp clientA clientB

central:central.cpp central.h
	$(CXX) $(CPPFLAGS) $^ -o $@

servert: serverT.cpp
	$(CXX) $(CPPFLAGS) $^ -o $@

servers: serverS.cpp
	$(CXX) $(CPPFLAGS) $^ -o $@

serverp: serverP.cpp minheap.h
	$(CXX) $(CPPFLAGS) $^ -o $@

clientA: clientA.cpp
	$(CXX) $(CPPFLAGS) $^ -o $@

clientB: clientB.cpp
	$(CXX) $(CPPFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm central
	rm serverT
	rm serverS
	rm serverP
	rm clientA
	rm clientB

serverC:
	./central

serverT:
	./servert

serverS:
	./servers

serverP:
	./serverp