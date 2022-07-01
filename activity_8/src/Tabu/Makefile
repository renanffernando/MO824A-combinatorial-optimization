###############################################################################################
######################### Compilação com suporte a múltiplos arquivos #########################
###############################################################################################

.PHONY: build clean

ARQ_MAIN = $(shell pwd | rev | cut -d '/' -f 1 | rev)

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

CPPFLAGS?=-g -O3 -DNDEBUG -Wall -Wextra -lm --std=c++17
GUROBIFLAGS?=-I/opt/gurobi951/linux64/include -L/opt/gurobi951/linux64/lib -lgurobi_c++ -lgurobi95

build: $(OBJECTS)
	g++ *.o -o $(ARQ_MAIN) $(CPPFLAGS) $(GUROBIFLAGS)

%.o: %.cpp
	g++ -c $< -o $@ $(CPPFLAGS)

softClean:
	rm $(ARQ_MAIN); rm -f *.o;

hardClean:
	rm $(ARQ_MAIN); rm -f *.o; rm -r out; rm output.png; rm gmon.out; rm *.out;
	