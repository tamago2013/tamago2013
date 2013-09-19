		
TARGET	:=visualizer
SHELL	:=bash
GCC	:=g++
REMOVE	:=rm -rf
MAKEDIR	:=mkdir -p
SRCS	:=$(TARGET).cpp


-include mk/subdir.mk
-include mk/launcher.mk

all:build

build:$(RELEASE_DIR)
	@cd ${RELEASE_DIR}; make -f ../${MAKEFILE_DIR}qtcreater.mk; cd -

clean:
	$(REMOVE) $(RELEASE_DIR) $(LAUNCHER)

$(RELEASE_DIR):
	@echo "make directory \"$(RELEASE_DIR)\""
	$(MAKEDIR) $@

.PHONY:all debug clean
