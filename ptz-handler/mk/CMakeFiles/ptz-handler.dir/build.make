# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/m-yoshida/TC13/tamago2013/ptz-handler

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/m-yoshida/TC13/tamago2013/ptz-handler/mk

# Include any dependencies generated for this target.
include CMakeFiles/ptz-handler.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ptz-handler.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ptz-handler.dir/flags.make

CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o: CMakeFiles/ptz-handler.dir/flags.make
CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o: ../src/ptz-handler.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/m-yoshida/TC13/tamago2013/ptz-handler/mk/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o -c /home/m-yoshida/TC13/tamago2013/ptz-handler/src/ptz-handler.cpp

CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/m-yoshida/TC13/tamago2013/ptz-handler/src/ptz-handler.cpp > CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.i

CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/m-yoshida/TC13/tamago2013/ptz-handler/src/ptz-handler.cpp -o CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.s

CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o.requires:
.PHONY : CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o.requires

CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o.provides: CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o.requires
	$(MAKE) -f CMakeFiles/ptz-handler.dir/build.make CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o.provides.build
.PHONY : CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o.provides

CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o.provides.build: CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o

# Object files for target ptz-handler
ptz__handler_OBJECTS = \
"CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o"

# External object files for target ptz-handler
ptz__handler_EXTERNAL_OBJECTS =

../Release/ptz-handler: CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o
../Release/ptz-handler: CMakeFiles/ptz-handler.dir/build.make
../Release/ptz-handler: CMakeFiles/ptz-handler.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../Release/ptz-handler"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ptz-handler.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ptz-handler.dir/build: ../Release/ptz-handler
.PHONY : CMakeFiles/ptz-handler.dir/build

CMakeFiles/ptz-handler.dir/requires: CMakeFiles/ptz-handler.dir/src/ptz-handler.cpp.o.requires
.PHONY : CMakeFiles/ptz-handler.dir/requires

CMakeFiles/ptz-handler.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ptz-handler.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ptz-handler.dir/clean

CMakeFiles/ptz-handler.dir/depend:
	cd /home/m-yoshida/TC13/tamago2013/ptz-handler/mk && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/m-yoshida/TC13/tamago2013/ptz-handler /home/m-yoshida/TC13/tamago2013/ptz-handler /home/m-yoshida/TC13/tamago2013/ptz-handler/mk /home/m-yoshida/TC13/tamago2013/ptz-handler/mk /home/m-yoshida/TC13/tamago2013/ptz-handler/mk/CMakeFiles/ptz-handler.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ptz-handler.dir/depend

