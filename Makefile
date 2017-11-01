# Makefile
# bushaofeng. 2011.8.27
# ver=1.0.0

CC=gcc
PP=g++
LIB=-lpthread

INCLDIR		= 	./

FPIC            =       -fPIC
WARN_LEVL       =       -Wall
FDEBUG          =       -g
SHARED			=		-shared
#STD_MODE		=		-std=c99
STD_MODE		=		-std=gnu99

EXTRA_OPT       =       $(FDEBUG)
EXTRA_OPT       +=      $(WARN_LEVL)
EXTRA_OPT       +=      $(FPIC)
EXTRA_OPT       +=      $(STD_MODE)
#EXTRA_OPT		+=		$(SHARED)
#EXTRA_OPT		+=		$(STD_MODE)

OBJDIR = .
SRCS = $(wildcard $(OBJDIR)/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS)) 

TARGET=libbs.a
$(TARGET):$(OBJS)
	echo $(OBJS)
	ar -r libbs.a $(OBJS)

$(OBJS):%.o: %.c
	$(CC) -c $(LDFLAGS) $(EXTRA_OPT) -I$(INCLDIR) $^ -o $@

clean:
	rm -rf *.a *.o
