# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/easy_test/ioctrl

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/easy_test/ioctrl/build

# Include any dependencies generated for this target.
include CMakeFiles/ioctrl_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ioctrl_test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ioctrl_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ioctrl_test.dir/flags.make

CMakeFiles/ioctrl_test.dir/ioctrl.c.o: CMakeFiles/ioctrl_test.dir/flags.make
CMakeFiles/ioctrl_test.dir/ioctrl.c.o: ../ioctrl.c
CMakeFiles/ioctrl_test.dir/ioctrl.c.o: CMakeFiles/ioctrl_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/easy_test/ioctrl/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ioctrl_test.dir/ioctrl.c.o"
	arm-rockchip830-linux-uclibcgnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/ioctrl_test.dir/ioctrl.c.o -MF CMakeFiles/ioctrl_test.dir/ioctrl.c.o.d -o CMakeFiles/ioctrl_test.dir/ioctrl.c.o -c /home/easy_test/ioctrl/ioctrl.c

CMakeFiles/ioctrl_test.dir/ioctrl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ioctrl_test.dir/ioctrl.c.i"
	arm-rockchip830-linux-uclibcgnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/easy_test/ioctrl/ioctrl.c > CMakeFiles/ioctrl_test.dir/ioctrl.c.i

CMakeFiles/ioctrl_test.dir/ioctrl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ioctrl_test.dir/ioctrl.c.s"
	arm-rockchip830-linux-uclibcgnueabihf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/easy_test/ioctrl/ioctrl.c -o CMakeFiles/ioctrl_test.dir/ioctrl.c.s

# Object files for target ioctrl_test
ioctrl_test_OBJECTS = \
"CMakeFiles/ioctrl_test.dir/ioctrl.c.o"

# External object files for target ioctrl_test
ioctrl_test_EXTERNAL_OBJECTS =

ioctrl_test: CMakeFiles/ioctrl_test.dir/ioctrl.c.o
ioctrl_test: CMakeFiles/ioctrl_test.dir/build.make
ioctrl_test: CMakeFiles/ioctrl_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/easy_test/ioctrl/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ioctrl_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ioctrl_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ioctrl_test.dir/build: ioctrl_test
.PHONY : CMakeFiles/ioctrl_test.dir/build

CMakeFiles/ioctrl_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ioctrl_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ioctrl_test.dir/clean

CMakeFiles/ioctrl_test.dir/depend:
	cd /home/easy_test/ioctrl/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/easy_test/ioctrl /home/easy_test/ioctrl /home/easy_test/ioctrl/build /home/easy_test/ioctrl/build /home/easy_test/ioctrl/build/CMakeFiles/ioctrl_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ioctrl_test.dir/depend

