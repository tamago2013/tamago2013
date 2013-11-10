#Makefile
SUBDIRS := cvusbcam_handler cvusbcam_viewer

.PHONY: all clean $(SUBDIRS)

all:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	$(MAKE) -C $$subdir || exit 1;\
	done

clean:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	$(MAKE) -C $$subdir clean || exit 1;\
	done
