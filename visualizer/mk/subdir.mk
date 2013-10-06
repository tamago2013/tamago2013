
# workspace directory
WORKSPACE			:=$(dir $(patsubst %/,%,$(PWD)) )

# target release directory
ifeq ($(MAKECMDGOALS),debug)
RELEASE_DIR			:=Debug/
else
ifeq ($(MAKECMDGOALS),clean-debug)
RELEASE_DIR			:=Debug/
else
RELEASE_DIR			:=Release/
endif
endif

# makefile
MAKEFILE_DIR			:=mk/


