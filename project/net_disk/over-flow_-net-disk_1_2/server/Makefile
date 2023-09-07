SRCS := $(wildcard *.c)
OBJS:=$(SRCS:%.c=%.o)
server:$(OBJS)
	gcc $^ -o $@ -lpthread -lcrypt
%.o:%.c 
	gcc -c $^ -o $@ -g
clean:
	$(RM) $(OBJS) server

