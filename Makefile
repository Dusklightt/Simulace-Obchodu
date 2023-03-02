FLAGS=-std=c++11 -pthread -Wall -O3 -g

all: simulation

simulation: main.o
	g++ $(FLAGS) $^ -o $@ -lsimlib -lm

%.o:%.cpp
	g++ $(FLAGS) -c $< -o $@ -I/usr/local/include -L/usr/local/lib64 -lsimlib -lm
	
run:	
	make && ./simulation

clean:
	rm simulation main.o

