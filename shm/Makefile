.PHONY:all
all:shm_client shm_server

shm_client:shm_client.cc
	g++ -I ../include/ $^ -o ./bin/$@ -g -std=c++11
shm_server:shm_server.cc
	g++ -I ../include/ $^ -o ./bin/$@ -g -std=c++11

.PHONY:clean
clean:
	rm -f ./bin/*
