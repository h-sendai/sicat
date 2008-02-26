CFLAGS = -g -Wall -O0
PROG = sicat
SRCS += sicat.c
SRCS += sicat.h
SRCS += host_list.c
SRCS += process_data.c
SRCS += socket.c
SRCS += status.c
SRCS += gf_time.c
SRCS += my_signal.h
SRCS += my_signal.c
SRCS += print_sockopt.c
NO_MAN = true
NO_OBJ = true
.include <bsd.prog.mk>
