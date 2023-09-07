SRCS:= $(wildcard *.c)
OBJS:=$(SRCS:%.c=%.o)
client:$(OBJS)
	gcc $^ -o $@ -lpthread
%.o:%.c 
	gcc -c $^ -o $@ -g
clean:
	$(RM) $(OBJS)
rmall:
	$(RM) $(OBJS) client
