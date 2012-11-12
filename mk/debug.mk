
_O_OPTION_	:=

_W_OPTION_	:=-Wall

_g_OPTION_	:=-g3 -pg

_l_OPTION_	:=$(patsubst %,-l%,$(LIB_LIST))

_I_OPTION_	:=$(patsubst %,-I$(WORKSPACE)%,$(HEADER_DIR_LIST))
