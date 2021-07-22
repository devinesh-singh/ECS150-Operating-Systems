SRCS=sshell.c 

OBJS=$(SRCS:.c=.o)

CC = gcc
CFLAGS = -g -Wall -Werror
TARGET = sshell

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

shell.o: shell.c shell.h
	$(CC) -g -Wall -c shell.c

clean:
	rm -f *.o *.out $(TARGET)
