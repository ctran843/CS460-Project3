P3.out : Project2.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o Object.o
	g++ -g -o P3.out Project2.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o Object.o

Project2.o : Project2.cpp SetLimits.h SyntacticalAnalyzer.h
	g++ -g -c Project2.cpp

SetLimits.o : SetLimits.cpp SetLimits.h
	g++ -g -c SetLimits.cpp

SyntacticalAnalyzer.o : SyntacticalAnalyzer.cpp SyntacticalAnalyzer.h LexicalAnalyzer.h CodeGenerator.h
	g++ -g -c SyntacticalAnalyzer.cpp

CodeGenerator.o : CodeGenerator.cpp CodeGenerator.h Object.h
	g++ -g -c CodeGenerator.cpp

Object.o : Object.cpp Object.h
	g++ -g -c Object.cpp

clean :
	rm [SP]*.o P2.out *.gch

submit : Project2.cpp LexicalAnalyzer.h SyntacticalAnalyzer.h SyntacticalAnalyzer.cpp CodeGenerator.cpp CodeGenerator.h Object.cpp Object.h makefile README.txt
	rm -rf Team27P3
	mkdir Team27P3
	cp Project2.cpp Team27P3
	cp SyntacticalAnalyzer.h Team27P3
	cp SyntacticalAnalyzer.cpp Team27P3
	cp LexicalAnalyzer.h Team27P3
	cp CodeGenerator.h Team27P3
	cp CodeGenerator.cpp Team27P3
	cp Object.h Team27P3
	cp Object.cpp Team27P3
	cp makefile Team27P3
	cp README.txt Team27P3
	tar cfvz Team27P3.tgz Team27P3
	cp Team27P3.tgz ~tiawatts/cs460drop
