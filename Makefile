CC=c++
FLAG=-Wall -std=c++14 -I./include/range-v3/include -I./include/catch/single_include -I./include/fmt
MAIN=lc3
TEST=test-exec
OBJS=token.o lexer.o ast.o parser.o object.o builtins.o modify.o quote_unquote.o eval.o macro_expansion.o interpret.o repl.o main.o

.PHONY: test clean all

build: $(OBJS)

all: $(OBJS) test main

token.o:
	$(CC) $(FLAG) -c ./src/token.hpp

lexer.o: token.o
	$(CC) $(FLAG) -c ./src/lexer.hpp

ast.o: token.o
	$(CC) $(FLAG) -c ./src/ast.hpp

parser.o: lexer.o
	$(CC) $(FLAG) -c ./src/parser.hpp

object.o:
	$(CC) $(FLAG) -c ./src/object.hpp

builtins.o:
	$(CC) $(FLAG) -c ./src/builtins.hpp

modify.o:
	$(CC) $(FLAG) -c ./src/modify.hpp

quote_unquote.o:
	$(CC) $(FLAG) -c ./src/quote_unquote.hpp

eval.o:
	$(CC) $(FLAG) -c ./src/eval.hpp

macro_expansion.o:
	$(CC) $(FLAG) -c ./src/macro_expansion.hpp

interpret.o:
	$(CC) $(FLAG) -c ./src/interpret.hpp

repl.o: lexer.o
	$(CC) $(FLAG) -c ./src/repl.hpp

main.o: repl.o
	$(CC) $(FLAG) -c ./src/main.cc

main: main.o
	$(CC) $(FLAG) -ledit main.o -o $(MAIN)

test:
	$(CC) $(FLAG) ./test/token_test.cc -o $(TEST) && ./$(TEST)
	$(CC) $(FLAG) ./test/lexer_test.cc -o $(TEST) && ./$(TEST)
	$(CC) $(FLAG) ./test/ast_test.cc -o $(TEST) && ./$(TEST)
	$(CC) $(FLAG) ./test/parser_test.cc -o $(TEST) && ./$(TEST)
	$(CC) $(FLAG) ./test/eval_test.cc -o $(TEST) && ./$(TEST)
	$(CC) $(FLAG) ./test/modify_test.cc -o $(TEST) && ./$(TEST)
	$(CC) $(FLAG) ./test/quote_unquote_test.cc -o $(TEST) && ./$(TEST)
	$(CC) $(FLAG) ./test/macro_expansion_test.cc -o $(TEST) && ./$(TEST)

clean:
	rm *.o $(MAIN) $(TEST)
