
DS9Devel=/data/mmti/pkg/tcl8.3.2

AST_VERSION=\"1.0\"

CC     = gcc -g
CFLAGS = -g -fPIC -fpcc-struct-return -I$(DS9Devel)/include -DAST_VERSION=$(AST_VERSION)
 
OBJS    = Ast.o		\
	  sla.o		\
	  SAOstrtod.o 	\
	  tclFormat.o	\
	  tclobj.o

LIBOBJS = $(OBJS)


LIB	= libast.a
SHARED	= libast.so

all     : $(OBJS) $(DIRS) $(LIB) $(SHARED) 

install : all lib shared

shared  : $(SHARED)
	cp libast.so ../lib/.

lib     : $(LIB)
	cp libast.a ../lib/.

$(SHARED): FORCE
	$(RM) $@
	gcc -shared -o $@ $(LIBOBJS) sla/libsla.a -lm
 
$(LIB)  : FORCE
	$(RM) $@
	$(AR) -cq $@ $(LIBOBJS)

iis	: FORCE
	cd iis; $(MAKE)


clean	: FORCE
	$(RM) core *~ *# *.bak
	cd iis; $(MAKE) clean

clobber	: clean
	rm -f *.o $(LIB)
	cd iis; $(MAKE) clobber


FORCE:
