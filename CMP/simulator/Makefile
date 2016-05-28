single_cycle: simulator.o instruction.o regfile.o memory.o
	g++ -o single_cycle simulator.o instruction.o regfile.o memory.o -g
simulator.o: simulator.cpp instruction.h regfile.h memory.h
	g++ -c simulator.cpp -g
instruction.o: instruction.cpp instruction.h regfile.h
	g++ -c instruction.cpp -g
regfile.o: regfile.cpp regfile.h
	g++ -c regfile.cpp -g
memory.o: memory.cpp memory.h
	g++ -c memory.cpp -g
clean:
	rm -f single_cycle simulator.o instruction.o regfile.o memory.o
