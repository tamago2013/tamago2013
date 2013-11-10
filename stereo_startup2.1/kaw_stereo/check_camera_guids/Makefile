CC=g++
CFLAGS = -O2 -Wall
LDLIBS = -ldc1394
OBJS = main.o
#カレントディレクトリ名のtryを作成する。
OUT=$(shell basename `pwd`)

.cpp.o:
	$(CC) $(CFLAGS) -c $< 

$(OUT):$(OBJS)
	$(CC) -o $@ $^ $(LDLIBS) 
	
clean:
	rm -f $(OUT) *.o
