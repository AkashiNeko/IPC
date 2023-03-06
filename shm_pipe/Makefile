.PHONY:all
all:shm_pipe_client shm_pipe_server

shm_pipe_client:shm_pipe_client.cc
	g++ -I ../include/ $^ -o ./bin/$@ -g -std=c++11
shm_pipe_server:shm_pipe_server.cc
	g++ -I ../include/ $^ -o ./bin/$@ -g -std=c++11

.PHONY:clean
clean:
	rm -f ./bin/*
