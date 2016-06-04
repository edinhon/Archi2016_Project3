CMP: simulator.o instruction.o regfile.o memory.o I_page_table.o I_TLB.o D_page_table.o D_TLB.o
	g++ -o CMP simulator.o instruction.o regfile.o memory.o I_page_table.o I_TLB.o D_page_table.o D_TLB.o -g
simulator.o: simulator.cpp instruction.h regfile.h memory.h I_page_table.h I_TLB.h D_page_table.h D_TLB.h
	g++ -c simulator.cpp -g
instruction.o: instruction.cpp instruction.h regfile.h I_page_table.h I_TLB.h memory.h D_page_table.h D_TLB.h
	g++ -c instruction.cpp -g
regfile.o: regfile.cpp regfile.h memory.h D_page_table.h D_TLB.h
	g++ -c regfile.cpp -g
memory.o: memory.cpp memory.h D_page_table.h D_TLB.h
	g++ -c memory.cpp -g
I_page_table.o: I_page_table.cpp I_page_table.h
	g++ -c I_page_table.cpp -g
I_TLB.o: I_TLB.cpp I_TLB.h
	g++ -c I_TLB.cpp -g
D_page_table.o: D_page_table.cpp D_page_table.h
	g++ -c D_page_table.cpp -g
D_TLB.o: D_TLB.cpp D_TLB.h
	g++ -c D_TLB.cpp -g
clean:
	rm -f CMP simulator.o instruction.o regfile.o memory.o I_page_table.o I_TLB.o D_page_table.o D_TLB.o
