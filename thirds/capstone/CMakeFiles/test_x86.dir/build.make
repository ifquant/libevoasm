# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /usr/bin/cmake3

# The command to remove a file.
RM = /usr/bin/cmake3 -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dev/libevoasm/thirds/capstone

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dev/libevoasm/thirds/capstone

# Include any dependencies generated for this target.
include CMakeFiles/test_x86.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_x86.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_x86.dir/flags.make

CMakeFiles/test_x86.dir/tests/test_x86.c.o: CMakeFiles/test_x86.dir/flags.make
CMakeFiles/test_x86.dir/tests/test_x86.c.o: tests/test_x86.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dev/libevoasm/thirds/capstone/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/test_x86.dir/tests/test_x86.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test_x86.dir/tests/test_x86.c.o   -c /home/dev/libevoasm/thirds/capstone/tests/test_x86.c

CMakeFiles/test_x86.dir/tests/test_x86.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test_x86.dir/tests/test_x86.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dev/libevoasm/thirds/capstone/tests/test_x86.c > CMakeFiles/test_x86.dir/tests/test_x86.c.i

CMakeFiles/test_x86.dir/tests/test_x86.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test_x86.dir/tests/test_x86.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dev/libevoasm/thirds/capstone/tests/test_x86.c -o CMakeFiles/test_x86.dir/tests/test_x86.c.s

CMakeFiles/test_x86.dir/tests/test_x86.c.o.requires:

.PHONY : CMakeFiles/test_x86.dir/tests/test_x86.c.o.requires

CMakeFiles/test_x86.dir/tests/test_x86.c.o.provides: CMakeFiles/test_x86.dir/tests/test_x86.c.o.requires
	$(MAKE) -f CMakeFiles/test_x86.dir/build.make CMakeFiles/test_x86.dir/tests/test_x86.c.o.provides.build
.PHONY : CMakeFiles/test_x86.dir/tests/test_x86.c.o.provides

CMakeFiles/test_x86.dir/tests/test_x86.c.o.provides.build: CMakeFiles/test_x86.dir/tests/test_x86.c.o


# Object files for target test_x86
test_x86_OBJECTS = \
"CMakeFiles/test_x86.dir/tests/test_x86.c.o"

# External object files for target test_x86
test_x86_EXTERNAL_OBJECTS =

test_x86: CMakeFiles/test_x86.dir/tests/test_x86.c.o
test_x86: CMakeFiles/test_x86.dir/build.make
test_x86: libcapstone.a
test_x86: CMakeFiles/test_x86.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dev/libevoasm/thirds/capstone/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test_x86"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_x86.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_x86.dir/build: test_x86

.PHONY : CMakeFiles/test_x86.dir/build

CMakeFiles/test_x86.dir/requires: CMakeFiles/test_x86.dir/tests/test_x86.c.o.requires

.PHONY : CMakeFiles/test_x86.dir/requires

CMakeFiles/test_x86.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_x86.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_x86.dir/clean

CMakeFiles/test_x86.dir/depend:
	cd /home/dev/libevoasm/thirds/capstone && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dev/libevoasm/thirds/capstone /home/dev/libevoasm/thirds/capstone /home/dev/libevoasm/thirds/capstone /home/dev/libevoasm/thirds/capstone /home/dev/libevoasm/thirds/capstone/CMakeFiles/test_x86.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_x86.dir/depend

