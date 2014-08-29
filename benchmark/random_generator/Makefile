EXEC = lcg

CC = gcc

CFLAGS = -std=c99 -g

$(EXEC) : $(EXEC).c
	$(CC) $(CFLAGS) -o $@ $^

gdb: $(EXEC)
	gdb $^ core

test: $(EXEC)
	gdb $^ -x ./test.in

run:
	@./lcg

clean: $(EXEC)
	rm $^	
