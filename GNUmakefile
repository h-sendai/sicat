CFLAGS = -g -Wall -O0
PROG = sicat
OBJS += sicat.o
OBJS += host_list.o
OBJS += process_data.o
OBJS += socket.o
OBJS += status.o
OBJS += gf_time.o
OBJS += my_signal.o
OBJS += print_sockopt.o

$(PROG): $(OBJS)

clean:
	rm -f $(PROG) $(OBJS) *.d .depend