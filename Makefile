CC=g++
MAIN=main.cpp
NAME=packets
INCLUDE=/usr/local/boost_1_55_0
LIB=/usr/local/lib

Release:
	$(CC) -std=c++11 -I $(INCLUDE) -Wl,-rpath=$(LIB) $(MAIN) -lboost_system -o $(NAME) 

Debug:
	$(CC) -Wall -g -std=c++11 -I $(INCLUDE) -Wl,-rpath=$(LIB) $(MAIN) -lboost_system -o $(NAME) 

clean:
	rm -rf *o $(NAME)
