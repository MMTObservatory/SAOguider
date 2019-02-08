
DS9Devel=/home/john/devel

CC     = gcc
CFLAGS = -g -fPIC -fpcc-struct-return -I$(DS9Devel)/include
 
OBJS = iis.o      iistcl.o   xim.o

all: $(OBJS)

clean	: FORCE
	rm -f core *~ *# *.bak

clobber	: clean
	rm -f *.o $(LIB)

FORCE:
