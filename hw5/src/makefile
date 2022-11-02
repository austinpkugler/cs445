CC = g++ -std=c++17 -g
BIN = c-
SRCS = $(BIN).y $(BIN).l
CPPS = *.cpp */*.cpp */*/*.cpp
HPPS = *.hpp */*.hpp */*/*.hpp
OBJS = lex.yy.o $(BIN).tab.o

$(BIN) : $(OBJS)
	$(CC) $(OBJS) $(CPPS) -o $(BIN)

$(BIN).tab.c : $(BIN).y
	bison -v -t -d $(BIN).y

lex.yy.c : $(BIN).l $(BIN).tab.c $(BIN).tab.h
	flex $(BIN).l

clean:
	rm lex.yy.* $(BIN) $(BIN).tab.* $(BIN).output

deepclean:
	rm lex.yy.* $(BIN) $(BIN).tab.* $(BIN).output $(BIN).tar

tar:
	tar -cvf $(BIN).tar $(CPPS) $(HPPS) makefile $(BIN).y $(BIN).l
