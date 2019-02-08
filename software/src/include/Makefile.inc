CFLAGS        = -DSTDC_HEADERS=1 	\
		-DHAVE_UNISTD_H=1	\
		-DHAVE_DIRENT_H=1	\
		-DHAVE_FCNTL_H=1 	\
		-DHAVE_MALLOC_H=1 	\
		-DHAVE_STDLIB_H=1 	\
		-DHAVE_MEMCPY		\
		-DHAVE_MEMCHR=1		\
		-DHAVE_STRERROR=1	\
		-Dlinux=1		\
		-D_REENTRANT		\
		-g			\
		-I ../../include

LDFLAGS       = -g                      \
		-L ../../../lib$(TARGET_ARCH)

MAKE_LIB_TARGET = ar crs $@ $?; mv $(TARGET) ../../../lib$(TARGET_ARCH)
MAKE_SERVER_TARGET = gcc $(LDFLAGS) $(TARGET_ARCH) -o $@ $? $(LIBS);mv $(TARGET) ../../../bin$(TARGET_ARCH)

top:	all$(TARGET_ARCH)

all:
	@echo 'must invoke as "make TARGET_ARCH=-m32" or "make TARGET_ARCH=-m64"'

all-m32 all-m64: $(TARGET) clean

$(TARGET): $(OBJECTS)
	$(MAKE_TARGET)

$(OBJECTS) : $(HEADERS)

clean:
	rm -f $(OBJECTS) $(TARGET)
