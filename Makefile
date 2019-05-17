default:
	$(MAKE) full

full: main.o buffer.o message.o parser.o record.o sockope.o table.o utils.o
	g++ main.o buffer.o message.o parser.o record.o sockope.o table.o utils.o -o main -lm
	$(RM) *.o

run:
	./main

clean:
	$(RM) *.o main
