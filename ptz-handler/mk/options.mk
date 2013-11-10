
#optimize option
_OPT_OPTION_	:=-O3

#warning option
_WRN_OPTION_	:=-Wall

#debug option
_DBG_OPTION_	:=

#preprocessor option
_PRE_OPTION_	:=-DNDEBUG=yes

#linker option
_LNK_OPTION_	:=$(patsubst %,-l%,$(LIBS)) /usr/local/lib/kaw_lib.a `pkg-config --libs opencv`

#header directory option
_HDIR_OPTION_	:=$(patsubst %,-I$(WORKSPACE)%,$(HEADER_DIR_LIST))

#library directory option
_LDIR_OPTION_	:= $(patsubst %,-L$(WORKSPACE)%,$(LIB_DIR_LIST))
