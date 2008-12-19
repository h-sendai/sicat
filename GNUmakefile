CFLAGS = -g -Wall -O2
PROG = sicat
OBJS += count_event_data.o
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
