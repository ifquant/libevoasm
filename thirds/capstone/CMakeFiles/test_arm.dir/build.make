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
include CMakeFiles/test_arm.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_arm.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_arm.dir/flags.make

CMakeFiles/test_arm.dir/tests/test_arm.c.o: CMakeFiles/test_arm.dir/flags.make
CMakeFiles/test_arm.dir/tests/test_arm.c.o: tests/test_arm.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dev/libevoasm/thirds/capstone/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/test_arm.dir/tests/test_arm.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test_arm.dir/tests/test_arm.c.o   -c /home/dev/libevoasm/thirds/capstone/tests/test_arm.c

CMakeFiles/test_arm.dir/tests/test_arm.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test_arm.dir/tests/test_arm.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dev/libevoasm/thirds/capstone/tests/test_arm.c > CMakeFiles/test_arm.dir/tests/test_arm.c.i

CMakeFiles/test_arm.dir/tests/test_arm.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test_arm.dir/tests/test_arm.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dev/libevoasm/thirds/capstone/tests/test_arm.c -o CMakeFiles/test_arm.dir/tests/test_arm.c.s

CMakeFiles/test_arm.dir/tests/test_arm.c.o.requires:

.PHONY : CMakeFiles/test_arm.dir/tests/test_arm.c.o.requires

CMakeFiles/test_arm.dir/tests/test_arm.c.o.provides: CMakeFiles/test_arm.dir/tests/test_arm.c.o.requires
	$(MAKE) -f CMakeFiles/test_arm.dir/build.make CMakeFiles/test_arm.dir/tests/test_arm.c.o.provides.build
.PHONY : CMakeFiles/test_arm.dir/tests/test_arm.c.o.provides

CMakeFiles/test_arm.dir/tests/test_arm.c.o.provides.build: CMakeFiles/test_arm.dir/tests/test_arm.c.o


# Object files for target test_arm
test_arm_OBJECTS = \
"CMakeFiles/test_arm.dir/tests/test_arm.c.o"

# External object files for target test_arm
test_arm_EXTERNAL_OBJECTS =

test_arm: CMakeFiles/test_arm.dir/tests/test_arm.c.o
test_arm: CMakeFiles/test_arm.dir/build.make
test_arm: libcapstone.a
test_arm: CMakeFiles/test_arm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dev/libevoasm/thirds/capstone/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test_arm"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_arm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_arm.dir/build: test_arm

.PHONY : CMakeFiles/test_arm.dir/build

CMakeFiles/test_arm.dir/requires: CMakeFiles/test_arm.dir/tests/test_arm.c.o.requires

.PHONY : CMakeFiles/test_arm.dir/requires

CMakeFiles/test_arm.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_arm.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_arm.dir/clean

CMakeFiles/test_arm.dir/depend:
	cd /home/dev/libevoasm/thirds/capstone && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dev/libevoasm/thirds/capstone /home/dev/libevoasm/thirds/capstone /home/dev/libevoasm/thirds/capstone /home/dev/libevoasm/thirds/capstone /home/dev/libevoasm/thirds/capstone/CMakeFiles/test_arm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_arm.dir/depend

