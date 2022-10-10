BIN = c-
CC = g++
CC_ARGS = -std=c++17

SRCS = $(BIN).y $(BIN).l
CC_SRCS = Util.cpp Node.cpp
HDRS = TokenData.hpp Util.hpp Node.hpp
OBJS = lex.yy.o $(BIN).tab.o

$(BIN) : $(OBJS)
	$(CC) $(OBJS) $(CC_SRCS) -o $(BIN) $(CC_ARGS)

lex.yy.c : $(BIN).l $(BIN).tab.h $(HDR)
	flex $(BIN).l

$(BIN).tab.h $(BIN).tab.c : $(BIN).y
	bison -v -t -d $(BIN).y

clean :
	rm -f *~ $(OBJS) $(BIN) lex.yy.c $(BIN).tab.h $(BIN).tab.c $(BIN).output test.out

tar : $(HDR) $(SRCS) makefile
	tar -cvf $(BIN).tar $(HDRS) $(SRCS) makefile