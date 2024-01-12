CC = g++
FLAGS = -std=c++17 --pedantic -Wall -Wextra

all: kompilator

kompilator: parser.o lexer.o io_handler.o ast_nodes.o semantic_analyser.o code_generator.o
	$(CC) $(FLAGS) -o kompilator parser.o lexer.o io_handler.o ast_nodes.o semantic_analyser.o code_generator.o

ast_nodes.o: ast_nodes.cpp
	$(CC) $(FLAGS) -c ast_nodes.cpp

code_generator.o: code_generator.cpp
	$(CC) $(FLAGS) -c code_generator.cpp

semantic_analyser.o: semantic_analyser.cpp
	$(CC) $(FLAGS) -c semantic_analyser.cpp

io_handler.o: io_handler.cpp
	$(CC) $(FLAGS) -c io_handler.cpp

lexer.o: lexer.c	
	$(CC) $(FLAGS) -c lexer.c

parser.o: parser.tab.c
	$(CC) $(FLAGS) -c parser.tab.c -o parser.o

lexer.c: lexer.l
	flex -o lexer.c lexer.l

parser.tab.c: parser.y
	bison -d parser.y

clean:
	rm -f *.o *~ *.c *.tab.c *.tab.h
	rm -f kompilator