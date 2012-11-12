
_O_OPTION_	:=-O3

_W_OPTION_	:=-Wall

_g_OPTION_	:=

_l_OPTION_	:=$(patsubst %,-l%,$(LIB_LIST))

_I_OPTION_	:=$(patsubst %,-I$(WORKSPACE)%,$(HEADER_DIR_LIST))
