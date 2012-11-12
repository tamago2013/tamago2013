
TARGET	:=$(notdir $(patsubst %/,%,$(PWD)) )
GCC		:=g++
REMOVE	:=rm -rf
MAKEDIR	:=mkdir -p
SRCS	:=$(TARGET).cpp


-include mk/subdir.mk
-include mk/objects.mk

ifeq ($(MAKECMDGOALS),debug)
-include mk/debug.mk
else
ifeq ($(MAKECMDGOALS),debugclean)
-include mk/debug.mk
else
-include mk/options.mk
endif
endif


# vpath
vpath
vpath %.cpp $(SRCS_DIR)
vpath %.o 	$(RELEASE_DIR)


.SUFFIXES: .o .cpp

all:build

build:$(RELEASE_DIR) $(_OBJS_)
	$(GCC) -o"$(RELEASE_DIR)$(TARGET)" $(patsubst %,$(RELEASE_DIR)%,$(_OBJS_)) $(_l_OPTION_)

rebuild:clean build

debug:rebuild

clean:
	$(REMOVE) $(patsubst %,$(RELEASE_DIR)%,$(_OBJS_)) $(RELEASE_DIR)$(TARGET)

clean-debug:
	echo "$(REMOVE) $(RELEASE_DIR)"

.cpp.o:
	g++ $(_I_OPTION_) $(_g_OPTION_) $(_O_OPTION_) $(_W_OPTION_) -c $< -o $(RELEASE_DIR)$@

$(RELEASE_DIR):
	@echo "make directory \"$(RELEASE_DIR)\""
	$(MAKEDIR) $@


.PHONY:all debug clean