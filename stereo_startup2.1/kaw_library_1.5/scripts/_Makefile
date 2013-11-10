CC=g++
CFLAGS = -O2 -Wall
LDLIBS = 
OBJS = $(shell basename `pwd`)

#.cpp.o:
#	$(CC) $(CFLAGS) -c $< 

$(OBJS).o:$(OBJS).cpp
	$(CC) $(CFLAGS) -c $<
	cp $(OBJS).hpp ../include/
	cp $(OBJS).o ../bin/
#	$(CC) -o $@ $^ $(LDLIBS) 
	
clean:
	rm -f *.o ../include/$(OBJS).hpp ../bin/$(OBJS).o
