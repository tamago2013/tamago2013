
#launcher shell script name
LAUNCHER		:=launcher

#lanch option
LAUNCH_OPTION	:=-g $(PWD)/psm-particle-evaluator.conf

#launch command
LAUNCH_CMD		:=./$(RELEASE_DIR)$(TARGET) $(LAUNCH_OPTION) "$$"@

#shell command interpreter
SHELL_INTRP		:=/bin/bash
