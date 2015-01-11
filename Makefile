CC=g++
#CC=clang++
CFLAGS=-c -std=c++11 -Wall -I.

all: calc

clean:
	rm calc
	rm *.o

calc: checker.o application.o exception.o main.o number.o expression.o parser.o token.o tokenizer.o
	$(CC) checker.o application.o exception.o main.o number.o expression.o parser.o token.o tokenizer.o -o calc

checker.o: checker/checker.cpp checker/checker.h core/exception.h tokenizer/token.h
	$(CC) $(CFLAGS) checker/checker.cpp -o checker.o

application.o: core/application.cpp core/number.h core/range.h checker/checker.h parser/expression.h parser/parser.h tokenizer/token.h tokenizer/token.h
	$(CC) $(CFLAGS) core/application.cpp -o application.o

exception.o: core/exception.cpp core/exception.h
	$(CC) $(CFLAGS) core/exception.cpp -o exception.o

main.o: core/main.cpp core/application.h
	$(CC) $(CFLAGS) core/main.cpp -o main.o

number.o: core/number.cpp core/number.h core/exception.h
	$(CC) $(CFLAGS) core/number.cpp -o number.o

expression.o: parser/expression.cpp parser/expression.h core/exception.h core/number.h core/range.h
	$(CC) $(CFLAGS) parser/expression.cpp -o expression.o

parser.o: parser/parser.cpp parser/expression.h parser/parser.h core/exception.h tokenizer/token.h
	$(CC) $(CFLAGS) parser/parser.cpp -o parser.o

token.o: tokenizer/token.cpp tokenizer/token.h core/exception.h core/number.h
	$(CC) $(CFLAGS) tokenizer/token.cpp -o token.o

tokenizer.o: tokenizer/tokenizer.cpp tokenizer/token.h tokenizer/tokenizer.h core/exception.h
	$(CC) $(CFLAGS) tokenizer/tokenizer.cpp -o tokenizer.o
