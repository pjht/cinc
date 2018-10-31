CC = gcc
CFLAGS = -Wall -g
SRCS = $(wildcard *.c */*.c */*/*.c)
OBJS = $(SRCS:.c=.o)
MAIN = cinc
.PHONY: clean
all: $(MAIN)
$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)
run: $(MAIN)
	./$(MAIN)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@
clean:
	$(RM) *.o *~ $(MAIN) out out.s
