#Makefile for kaw_lib
SUBDIRS := FrameRate FrameRateClass TxtConfigurator SSM-Image GLMetaseq2
OUT = kaw_lib

.PHONY: all clean $(SUBDIRS)

all:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	$(MAKE) -C $$subdir || exit 1;\
	done
	ar rsv ./lib/$(OUT).a ./bin/*.o

clean:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	$(MAKE) -C $$subdir clean || exit 1;\
	done
	rm ./lib/$(OUT).a

install:
	mkdir -p /usr/local/include/$(OUT)
	cp ./include/* /usr/local/include/$(OUT)/
	cp ./lib/$(OUT).a /usr/local/lib

uninstall:
	rm -rf /usr/local/include/$(OUT)
	rm /usr/local/lib/$(OUT).a

