top:	all$(TARGET_ARCH)

all:
	@echo 'must invoke as "make TARGET_ARCH=-m32" or "make TARGET_ARCH=-m64"'

all-m32 all-m64:
	for dir in $(SUBDIRS); do \
	    $(MAKE) -C $$dir; \
	done

clean:
	for dir in $(SUBDIRS); do \
	    $(MAKE) -C $$dir $@; \
	done

cleanbin:
	for dir in $(BINDIRS); do \
	    $(MAKE) -C $$dir $@; \
	done

