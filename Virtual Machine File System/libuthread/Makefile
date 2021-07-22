target := p2
objs:= queue.o uthread.o context.o preempt.o

CC := gcc
CFLAGS := -Wall -Werror
CFLAGS += -g

# Target library
lib := libuthread.a queue.o uthread.o context.o preempt.o

all: $(target)

# Dep tracking must be below the 'all' rule
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

p2: $(objs)
	$(CC) $(CFLAGS) -o $@ $^

lib: $(lib)
	ar rcs $(lib)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)

clean:
	rm -f *.o *.d
