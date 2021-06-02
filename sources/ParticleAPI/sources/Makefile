######################################################################
# Particle System API
#
# Copyright 1998-2006 by David K. McAllister.
#
######################################################################

C++ = g++

COPT = -O3

CFLAGS = $(COPT) $(COMPFLAGS) -I. -I../Particle

POBJS =ActionsAPI.o Actions.o OtherAPI.o PInternalState.o

ALL = libParticle.a

all: $(ALL)

.cpp.o:
	$(C++) $(CFLAGS) -c $<

libParticle.a: $(POBJS)
	rm -f $@
	ar clq $@ $(POBJS)

clean:
	rm -f $(ALL)
	rm -f *.o
