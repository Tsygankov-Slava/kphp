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
CMAKE_COMMAND = /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/cmake

# The command to remove a file.
RM = /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build

# Include any dependencies generated for this target.
include kphp/CMakeFiles/binlog_src.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include kphp/CMakeFiles/binlog_src.dir/compiler_depend.make

# Include the progress variables for this target.
include kphp/CMakeFiles/binlog_src.dir/progress.make

# Include the compile flags for this target's objects.
include kphp/CMakeFiles/binlog_src.dir/flags.make

kphp/CMakeFiles/binlog_src.dir/common/binlog/kdb-binlog-common.cpp.o: kphp/CMakeFiles/binlog_src.dir/flags.make
kphp/CMakeFiles/binlog_src.dir/common/binlog/kdb-binlog-common.cpp.o: ../kphp/common/binlog/kdb-binlog-common.cpp
kphp/CMakeFiles/binlog_src.dir/common/binlog/kdb-binlog-common.cpp.o: kphp/CMakeFiles/binlog_src.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object kphp/CMakeFiles/binlog_src.dir/common/binlog/kdb-binlog-common.cpp.o"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT kphp/CMakeFiles/binlog_src.dir/common/binlog/kdb-binlog-common.cpp.o -MF CMakeFiles/binlog_src.dir/common/binlog/kdb-binlog-common.cpp.o.d -o CMakeFiles/binlog_src.dir/common/binlog/kdb-binlog-common.cpp.o -c /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/kdb-binlog-common.cpp

kphp/CMakeFiles/binlog_src.dir/common/binlog/kdb-binlog-common.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/binlog_src.dir/common/binlog/kdb-binlog-common.cpp.i"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/kdb-binlog-common.cpp > CMakeFiles/binlog_src.dir/common/binlog/kdb-binlog-common.cpp.i

kphp/CMakeFiles/binlog_src.dir/common/binlog/kdb-binlog-common.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/binlog_src.dir/common/binlog/kdb-binlog-common.cpp.s"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/kdb-binlog-common.cpp -o CMakeFiles/binlog_src.dir/common/binlog/kdb-binlog-common.cpp.s

kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer.cpp.o: kphp/CMakeFiles/binlog_src.dir/flags.make
kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer.cpp.o: ../kphp/common/binlog/binlog-buffer.cpp
kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer.cpp.o: kphp/CMakeFiles/binlog_src.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer.cpp.o"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer.cpp.o -MF CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer.cpp.o.d -o CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer.cpp.o -c /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/binlog-buffer.cpp

kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer.cpp.i"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/binlog-buffer.cpp > CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer.cpp.i

kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer.cpp.s"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/binlog-buffer.cpp -o CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer.cpp.s

kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-aio.cpp.o: kphp/CMakeFiles/binlog_src.dir/flags.make
kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-aio.cpp.o: ../kphp/common/binlog/binlog-buffer-aio.cpp
kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-aio.cpp.o: kphp/CMakeFiles/binlog_src.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-aio.cpp.o"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-aio.cpp.o -MF CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-aio.cpp.o.d -o CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-aio.cpp.o -c /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/binlog-buffer-aio.cpp

kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-aio.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-aio.cpp.i"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/binlog-buffer-aio.cpp > CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-aio.cpp.i

kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-aio.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-aio.cpp.s"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/binlog-buffer-aio.cpp -o CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-aio.cpp.s

kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-rotation-points.cpp.o: kphp/CMakeFiles/binlog_src.dir/flags.make
kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-rotation-points.cpp.o: ../kphp/common/binlog/binlog-buffer-rotation-points.cpp
kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-rotation-points.cpp.o: kphp/CMakeFiles/binlog_src.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-rotation-points.cpp.o"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-rotation-points.cpp.o -MF CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-rotation-points.cpp.o.d -o CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-rotation-points.cpp.o -c /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/binlog-buffer-rotation-points.cpp

kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-rotation-points.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-rotation-points.cpp.i"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/binlog-buffer-rotation-points.cpp > CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-rotation-points.cpp.i

kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-rotation-points.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-rotation-points.cpp.s"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/binlog-buffer-rotation-points.cpp -o CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-rotation-points.cpp.s

kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-replay.cpp.o: kphp/CMakeFiles/binlog_src.dir/flags.make
kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-replay.cpp.o: ../kphp/common/binlog/binlog-buffer-replay.cpp
kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-replay.cpp.o: kphp/CMakeFiles/binlog_src.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-replay.cpp.o"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-replay.cpp.o -MF CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-replay.cpp.o.d -o CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-replay.cpp.o -c /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/binlog-buffer-replay.cpp

kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-replay.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-replay.cpp.i"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/binlog-buffer-replay.cpp > CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-replay.cpp.i

kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-replay.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-replay.cpp.s"
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && /opt/KasperskyOS-Community-Edition-1.1.1.13/toolchain/bin/aarch64-kos-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/common/binlog/binlog-buffer-replay.cpp -o CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-replay.cpp.s

binlog_src: kphp/CMakeFiles/binlog_src.dir/common/binlog/kdb-binlog-common.cpp.o
binlog_src: kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer.cpp.o
binlog_src: kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-aio.cpp.o
binlog_src: kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-rotation-points.cpp.o
binlog_src: kphp/CMakeFiles/binlog_src.dir/common/binlog/binlog-buffer-replay.cpp.o
binlog_src: kphp/CMakeFiles/binlog_src.dir/build.make
.PHONY : binlog_src

# Rule to build all files generated by this target.
kphp/CMakeFiles/binlog_src.dir/build: binlog_src
.PHONY : kphp/CMakeFiles/binlog_src.dir/build

kphp/CMakeFiles/binlog_src.dir/clean:
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp && $(CMAKE_COMMAND) -P CMakeFiles/binlog_src.dir/cmake_clean.cmake
.PHONY : kphp/CMakeFiles/binlog_src.dir/clean

kphp/CMakeFiles/binlog_src.dir/depend:
	cd /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp /opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/kphp/CMakeFiles/binlog_src.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : kphp/CMakeFiles/binlog_src.dir/depend

