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
include CMakeFiles/cstool.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cstool.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cstool.dir/flags.make

CMakeFiles/cstool.dir/cstool/cstool.c.o: CMakeFiles/cstool.dir/flags.make
CMakeFiles/cstool.dir/cstool/cstool.c.o: cstool/cstool.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dev/libevoasm/thirds/capstone/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/cstool.dir/cstool/cstool.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/cstool.dir/cstool/cstool.c.o   -c /home/dev/libevoasm/thirds/capstone/cstool/cstool.c

CMakeFiles/cstool.dir/cstool/cstool.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cstool.dir/cstool/cstool.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dev/libevoasm/thirds/capstone/cstool/cstool.c > CMakeFiles/cstool.dir/cstool/cstool.c.i

CMakeFiles/cstool.dir/cstool/cstool.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cstool.dir/cstool/cstool.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dev/libevoasm/thirds/capstone/cstool/cstool.c -o CMakeFiles/cstool.dir/cstool/cstool.c.s

CMakeFiles/cstool.dir/cstool/cstool.c.o.requires:

.PHONY : CMakeFiles/cstool.dir/cstool/cstool.c.o.requires

CMakeFiles/cstool.dir/cstool/cstool.c.o.provides: CMakeFiles/cstool.dir/cstool/cstool.c.o.requires
	$(MAKE) -f CMakeFiles/cstool.dir/build.make CMakeFiles/cstool.dir/cstool/cstool.c.o.provides.build
.PHONY : CMakeFiles/cstool.dir/cstool/cstool.c.o.provides

CMakeFiles/cstool.dir/cstool/cstool.c.o.provides.build: CMakeFiles/cstool.dir/cstool/cstool.c.o


CMakeFiles/cstool.dir/cstool/cstool_arm.c.o: CMakeFiles/cstool.dir/flags.make
CMakeFiles/cstool.dir/cstool/cstool_arm.c.o: cstool/cstool_arm.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dev/libevoasm/thirds/capstone/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/cstool.dir/cstool/cstool_arm.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/cstool.dir/cstool/cstool_arm.c.o   -c /home/dev/libevoasm/thirds/capstone/cstool/cstool_arm.c

CMakeFiles/cstool.dir/cstool/cstool_arm.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cstool.dir/cstool/cstool_arm.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dev/libevoasm/thirds/capstone/cstool/cstool_arm.c > CMakeFiles/cstool.dir/cstool/cstool_arm.c.i

CMakeFiles/cstool.dir/cstool/cstool_arm.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cstool.dir/cstool/cstool_arm.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dev/libevoasm/thirds/capstone/cstool/cstool_arm.c -o CMakeFiles/cstool.dir/cstool/cstool_arm.c.s

CMakeFiles/cstool.dir/cstool/cstool_arm.c.o.requires:

.PHONY : CMakeFiles/cstool.dir/cstool/cstool_arm.c.o.requires

CMakeFiles/cstool.dir/cstool/cstool_arm.c.o.provides: CMakeFiles/cstool.dir/cstool/cstool_arm.c.o.requires
	$(MAKE) -f CMakeFiles/cstool.dir/build.make CMakeFiles/cstool.dir/cstool/cstool_arm.c.o.provides.build
.PHONY : CMakeFiles/cstool.dir/cstool/cstool_arm.c.o.provides

CMakeFiles/cstool.dir/cstool/cstool_arm.c.o.provides.build: CMakeFiles/cstool.dir/cstool/cstool_arm.c.o


CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o: CMakeFiles/cstool.dir/flags.make
CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o: cstool/cstool_arm64.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dev/libevoasm/thirds/capstone/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o   -c /home/dev/libevoasm/thirds/capstone/cstool/cstool_arm64.c

CMakeFiles/cstool.dir/cstool/cstool_arm64.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cstool.dir/cstool/cstool_arm64.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dev/libevoasm/thirds/capstone/cstool/cstool_arm64.c > CMakeFiles/cstool.dir/cstool/cstool_arm64.c.i

CMakeFiles/cstool.dir/cstool/cstool_arm64.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cstool.dir/cstool/cstool_arm64.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dev/libevoasm/thirds/capstone/cstool/cstool_arm64.c -o CMakeFiles/cstool.dir/cstool/cstool_arm64.c.s

CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o.requires:

.PHONY : CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o.requires

CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o.provides: CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o.requires
	$(MAKE) -f CMakeFiles/cstool.dir/build.make CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o.provides.build
.PHONY : CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o.provides

CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o.provides.build: CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o


CMakeFiles/cstool.dir/cstool/cstool_mips.c.o: CMakeFiles/cstool.dir/flags.make
CMakeFiles/cstool.dir/cstool/cstool_mips.c.o: cstool/cstool_mips.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dev/libevoasm/thirds/capstone/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/cstool.dir/cstool/cstool_mips.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/cstool.dir/cstool/cstool_mips.c.o   -c /home/dev/libevoasm/thirds/capstone/cstool/cstool_mips.c

CMakeFiles/cstool.dir/cstool/cstool_mips.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cstool.dir/cstool/cstool_mips.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dev/libevoasm/thirds/capstone/cstool/cstool_mips.c > CMakeFiles/cstool.dir/cstool/cstool_mips.c.i

CMakeFiles/cstool.dir/cstool/cstool_mips.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cstool.dir/cstool/cstool_mips.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dev/libevoasm/thirds/capstone/cstool/cstool_mips.c -o CMakeFiles/cstool.dir/cstool/cstool_mips.c.s

CMakeFiles/cstool.dir/cstool/cstool_mips.c.o.requires:

.PHONY : CMakeFiles/cstool.dir/cstool/cstool_mips.c.o.requires

CMakeFiles/cstool.dir/cstool/cstool_mips.c.o.provides: CMakeFiles/cstool.dir/cstool/cstool_mips.c.o.requires
	$(MAKE) -f CMakeFiles/cstool.dir/build.make CMakeFiles/cstool.dir/cstool/cstool_mips.c.o.provides.build
.PHONY : CMakeFiles/cstool.dir/cstool/cstool_mips.c.o.provides

CMakeFiles/cstool.dir/cstool/cstool_mips.c.o.provides.build: CMakeFiles/cstool.dir/cstool/cstool_mips.c.o


CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o: CMakeFiles/cstool.dir/flags.make
CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o: cstool/cstool_ppc.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dev/libevoasm/thirds/capstone/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o   -c /home/dev/libevoasm/thirds/capstone/cstool/cstool_ppc.c

CMakeFiles/cstool.dir/cstool/cstool_ppc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cstool.dir/cstool/cstool_ppc.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dev/libevoasm/thirds/capstone/cstool/cstool_ppc.c > CMakeFiles/cstool.dir/cstool/cstool_ppc.c.i

CMakeFiles/cstool.dir/cstool/cstool_ppc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cstool.dir/cstool/cstool_ppc.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dev/libevoasm/thirds/capstone/cstool/cstool_ppc.c -o CMakeFiles/cstool.dir/cstool/cstool_ppc.c.s

CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o.requires:

.PHONY : CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o.requires

CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o.provides: CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o.requires
	$(MAKE) -f CMakeFiles/cstool.dir/build.make CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o.provides.build
.PHONY : CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o.provides

CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o.provides.build: CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o


CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o: CMakeFiles/cstool.dir/flags.make
CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o: cstool/cstool_sparc.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dev/libevoasm/thirds/capstone/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o   -c /home/dev/libevoasm/thirds/capstone/cstool/cstool_sparc.c

CMakeFiles/cstool.dir/cstool/cstool_sparc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cstool.dir/cstool/cstool_sparc.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dev/libevoasm/thirds/capstone/cstool/cstool_sparc.c > CMakeFiles/cstool.dir/cstool/cstool_sparc.c.i

CMakeFiles/cstool.dir/cstool/cstool_sparc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cstool.dir/cstool/cstool_sparc.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dev/libevoasm/thirds/capstone/cstool/cstool_sparc.c -o CMakeFiles/cstool.dir/cstool/cstool_sparc.c.s

CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o.requires:

.PHONY : CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o.requires

CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o.provides: CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o.requires
	$(MAKE) -f CMakeFiles/cstool.dir/build.make CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o.provides.build
.PHONY : CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o.provides

CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o.provides.build: CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o


CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o: CMakeFiles/cstool.dir/flags.make
CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o: cstool/cstool_systemz.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dev/libevoasm/thirds/capstone/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o   -c /home/dev/libevoasm/thirds/capstone/cstool/cstool_systemz.c

CMakeFiles/cstool.dir/cstool/cstool_systemz.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cstool.dir/cstool/cstool_systemz.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dev/libevoasm/thirds/capstone/cstool/cstool_systemz.c > CMakeFiles/cstool.dir/cstool/cstool_systemz.c.i

CMakeFiles/cstool.dir/cstool/cstool_systemz.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cstool.dir/cstool/cstool_systemz.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dev/libevoasm/thirds/capstone/cstool/cstool_systemz.c -o CMakeFiles/cstool.dir/cstool/cstool_systemz.c.s

CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o.requires:

.PHONY : CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o.requires

CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o.provides: CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o.requires
	$(MAKE) -f CMakeFiles/cstool.dir/build.make CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o.provides.build
.PHONY : CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o.provides

CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o.provides.build: CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o


CMakeFiles/cstool.dir/cstool/cstool_x86.c.o: CMakeFiles/cstool.dir/flags.make
CMakeFiles/cstool.dir/cstool/cstool_x86.c.o: cstool/cstool_x86.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dev/libevoasm/thirds/capstone/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/cstool.dir/cstool/cstool_x86.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/cstool.dir/cstool/cstool_x86.c.o   -c /home/dev/libevoasm/thirds/capstone/cstool/cstool_x86.c

CMakeFiles/cstool.dir/cstool/cstool_x86.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cstool.dir/cstool/cstool_x86.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dev/libevoasm/thirds/capstone/cstool/cstool_x86.c > CMakeFiles/cstool.dir/cstool/cstool_x86.c.i

CMakeFiles/cstool.dir/cstool/cstool_x86.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cstool.dir/cstool/cstool_x86.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dev/libevoasm/thirds/capstone/cstool/cstool_x86.c -o CMakeFiles/cstool.dir/cstool/cstool_x86.c.s

CMakeFiles/cstool.dir/cstool/cstool_x86.c.o.requires:

.PHONY : CMakeFiles/cstool.dir/cstool/cstool_x86.c.o.requires

CMakeFiles/cstool.dir/cstool/cstool_x86.c.o.provides: CMakeFiles/cstool.dir/cstool/cstool_x86.c.o.requires
	$(MAKE) -f CMakeFiles/cstool.dir/build.make CMakeFiles/cstool.dir/cstool/cstool_x86.c.o.provides.build
.PHONY : CMakeFiles/cstool.dir/cstool/cstool_x86.c.o.provides

CMakeFiles/cstool.dir/cstool/cstool_x86.c.o.provides.build: CMakeFiles/cstool.dir/cstool/cstool_x86.c.o


CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o: CMakeFiles/cstool.dir/flags.make
CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o: cstool/cstool_xcore.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dev/libevoasm/thirds/capstone/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o   -c /home/dev/libevoasm/thirds/capstone/cstool/cstool_xcore.c

CMakeFiles/cstool.dir/cstool/cstool_xcore.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cstool.dir/cstool/cstool_xcore.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dev/libevoasm/thirds/capstone/cstool/cstool_xcore.c > CMakeFiles/cstool.dir/cstool/cstool_xcore.c.i

CMakeFiles/cstool.dir/cstool/cstool_xcore.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cstool.dir/cstool/cstool_xcore.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dev/libevoasm/thirds/capstone/cstool/cstool_xcore.c -o CMakeFiles/cstool.dir/cstool/cstool_xcore.c.s

CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o.requires:

.PHONY : CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o.requires

CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o.provides: CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o.requires
	$(MAKE) -f CMakeFiles/cstool.dir/build.make CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o.provides.build
.PHONY : CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o.provides

CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o.provides.build: CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o


# Object files for target cstool
cstool_OBJECTS = \
"CMakeFiles/cstool.dir/cstool/cstool.c.o" \
"CMakeFiles/cstool.dir/cstool/cstool_arm.c.o" \
"CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o" \
"CMakeFiles/cstool.dir/cstool/cstool_mips.c.o" \
"CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o" \
"CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o" \
"CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o" \
"CMakeFiles/cstool.dir/cstool/cstool_x86.c.o" \
"CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o"

# External object files for target cstool
cstool_EXTERNAL_OBJECTS =

cstool: CMakeFiles/cstool.dir/cstool/cstool.c.o
cstool: CMakeFiles/cstool.dir/cstool/cstool_arm.c.o
cstool: CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o
cstool: CMakeFiles/cstool.dir/cstool/cstool_mips.c.o
cstool: CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o
cstool: CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o
cstool: CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o
cstool: CMakeFiles/cstool.dir/cstool/cstool_x86.c.o
cstool: CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o
cstool: CMakeFiles/cstool.dir/build.make
cstool: libcapstone.a
cstool: CMakeFiles/cstool.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dev/libevoasm/thirds/capstone/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking C executable cstool"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cstool.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cstool.dir/build: cstool

.PHONY : CMakeFiles/cstool.dir/build

CMakeFiles/cstool.dir/requires: CMakeFiles/cstool.dir/cstool/cstool.c.o.requires
CMakeFiles/cstool.dir/requires: CMakeFiles/cstool.dir/cstool/cstool_arm.c.o.requires
CMakeFiles/cstool.dir/requires: CMakeFiles/cstool.dir/cstool/cstool_arm64.c.o.requires
CMakeFiles/cstool.dir/requires: CMakeFiles/cstool.dir/cstool/cstool_mips.c.o.requires
CMakeFiles/cstool.dir/requires: CMakeFiles/cstool.dir/cstool/cstool_ppc.c.o.requires
CMakeFiles/cstool.dir/requires: CMakeFiles/cstool.dir/cstool/cstool_sparc.c.o.requires
CMakeFiles/cstool.dir/requires: CMakeFiles/cstool.dir/cstool/cstool_systemz.c.o.requires
CMakeFiles/cstool.dir/requires: CMakeFiles/cstool.dir/cstool/cstool_x86.c.o.requires
CMakeFiles/cstool.dir/requires: CMakeFiles/cstool.dir/cstool/cstool_xcore.c.o.requires

.PHONY : CMakeFiles/cstool.dir/requires

CMakeFiles/cstool.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cstool.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cstool.dir/clean

CMakeFiles/cstool.dir/depend:
	cd /home/dev/libevoasm/thirds/capstone && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dev/libevoasm/thirds/capstone /home/dev/libevoasm/thirds/capstone /home/dev/libevoasm/thirds/capstone /home/dev/libevoasm/thirds/capstone /home/dev/libevoasm/thirds/capstone/CMakeFiles/cstool.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cstool.dir/depend

