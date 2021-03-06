#Makefile Variables
CC = g++     
LD = $(CC)
CCFLAGS = -Wall -Wno-deprecated --std=c++11 -g -c   
LDFLAGS = -Wall --std=c++11 -Wno-deprecated -g
OBJS = main.o Intersection.o Lane.o Section.o Clock.o Parser.o TrafficLight.o Vehicle.o Random.o Statistics.o
TESTS = SectionTester ParserTester #MainTester 
################################################################################

#compiles main from .o files
main: $(OBJS) 
	$(LD) $(LDFLAGS) $(OBJS) -o $@

r: 									#runs main
	./main simulation.config 0

d: 									#runs debug main
	./main simulation.config 1

gdb:
	gdb ./main 

v:
	valgrind -v --leak-check=full ./main 

clean: 							#removes executable and output file
	rm main

reallyclean:  	#cleans and removes all .o files
	rm *.o

o:
	vim main.cpp +":tabf Section.cpp" +":vsplit Section.h" +":tabf Lane.cpp" +":vsplit Lane.h" +":tabf Intersection.cpp" +":vsplit Intersection.h" +":tabf Vehicle.cpp" +":vsplit Vehicle.h"

test: $(TESTS) 
	./SectionTester
	./ParserTester
	#./MainTester

################################################################################
# O Compilers -- reduces recompilation
################################################################################
main.o: main.cpp Intersection.cpp TrafficLight.cpp Clock.cpp Random.cpp Parser.cpp Vehicle.cpp Statistics.cpp
	$(CC) $(CCFLAGS) $<

Intersection.o: Intersection.cpp Lane.cpp Section.cpp
	$(CC) $(CCFLAGS) $<

Lane.o: Lane.cpp Section.cpp
	$(CC) $(CCFLAGS) $<

Section.o: Section.cpp 
	$(CC) $(CCFLAGS) $<
	
Clock.o: Clock.cpp 
	$(CC) $(CCFLAGS) $<

Random.o: Random.cpp 
	$(CC) $(CCFLAGS) $<

Statistics.o: Statistics.cpp 
	$(CC) $(CCFLAGS) $<

Vehicle.o: Vehicle.cpp Section.cpp Lane.cpp Random.cpp Intersection.cpp
	$(CC) $(CCFLAGS) $<

################################################################################
# Tester Compilers -- 
################################################################################
SectionTester: SectionTester.cpp Section.cpp
	$(CC) $(LDFLAGS) SectionTester.cpp Section.cpp -o $@

ParserTester: Parser.cpp
	$(CC) $(LDFLAGS) Parser.cpp -o $@

MainTester: main.cpp # not used anymore
	$(CC) $(LDFLAGS) main.cpp -o $@
