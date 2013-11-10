CC=g++
CFLAGS = -O2 -Wall
LDLIBS = /usr/local/lib/kaw_lib.a `pkg-config --libs opencv` -lssm
OBJS = main.o
OUT = cv2usbcam_handler

.cpp.o:
	$(CC) $(CFLAGS) -c $< 

$(OUT):$(OBJS)
	$(CC) -o $@ $^ $(LDLIBS)
	
clean:
	rm -f $(OUT) *.o
