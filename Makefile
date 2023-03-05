.PHONY:all
all:client server

client:client.cc
	g++ $^ -o ./bin/$@ -g -std=c++11
server:server.cc
	g++ $^ -o ./bin/$@ -g -std=c++11

.PHONY:clean
clean:
	rm -f ./bin/*
