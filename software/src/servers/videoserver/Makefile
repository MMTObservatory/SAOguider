HEADERS = iproc.h
OBJECTS = blitter.o bswap.o camera_videoserver.o center__c.o fake.o framegrabber.o guide.o handle1.o headsimple.o processadd.o quickcenter.o smooth.o videoserver.o
LIBS    = -lfuntools -lmsg -lutil -lpthread -lm `GraphicsMagick-config --ldflags --libs`
CPPFLAGS = -O `GraphicsMagick-config --cppflags`
TARGET  = camera_videoserver
MAKE_TARGET = $(MAKE_SERVER_TARGET)
include ../../include/Makefile.inc
