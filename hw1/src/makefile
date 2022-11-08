CC = g++
SRCS = c-.y c-.l
HDRS = Util.hpp TokenData.hpp
OBJS = lex.yy.o c-.tab.o

c- : $(OBJS)
	$(CC) $(OBJS) Util.cpp -o c- -std=c++17

lex.yy.c : c-.l c-.tab.h $(HDR)
	flex c-.l

c-.tab.h c-.tab.c : c-.y
	bison -v -t -d c-.y

clean :
	rm -f *~ c- c-.output c-.tab.h c-.tab.c lex.yy.c $(OBJS)

tar : $(HDR) $(SRCS) makefile
	tar -cvf c-.tar $(HDRS) $(SRCS) makefile
