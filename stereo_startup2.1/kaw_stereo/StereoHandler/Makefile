CC=g++
CFLAGS = -O2 -Wall
STLIBS = /usr/local/lib/kaw_lib.a
LDLIBS = `pkg-config --libs opencv` -lssm -ldc1394
OBJS = main.o
OUT=StereoHandler

.cpp.o:
	$(CC) $(CFLAGS) -c $<

$(OUT):$(OBJS)
	$(CC) -o $@ $^ $(STLIBS) $(LDLIBS)
	
clean:
	rm -f $(OUT) *.o *~
