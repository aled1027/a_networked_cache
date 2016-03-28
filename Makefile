# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/alex/SpiderOak Hive/all_notes/spring/systems/a_networked_cache"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/alex/SpiderOak Hive/all_notes/spring/systems/a_networked_cache"

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running interactive CMake command-line interface..."
	/usr/bin/cmake -i .
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start "/home/alex/SpiderOak Hive/all_notes/spring/systems/a_networked_cache/CMakeFiles" "/home/alex/SpiderOak Hive/all_notes/spring/systems/a_networked_cache/CMakeFiles/progress.marks"
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start "/home/alex/SpiderOak Hive/all_notes/spring/systems/a_networked_cache/CMakeFiles" 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named a_networked_cache

# Build rule for target.
a_networked_cache: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 a_networked_cache
.PHONY : a_networked_cache

# fast build rule for target.
a_networked_cache/fast:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/build
.PHONY : a_networked_cache/fast

#=============================================================================
# Target rules for targets named gdb_client

# Build rule for target.
gdb_client: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 gdb_client
.PHONY : gdb_client

# fast build rule for target.
gdb_client/fast:
	$(MAKE) -f CMakeFiles/gdb_client.dir/build.make CMakeFiles/gdb_client.dir/build
.PHONY : gdb_client/fast

#=============================================================================
# Target rules for targets named gdb_server

# Build rule for target.
gdb_server: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 gdb_server
.PHONY : gdb_server

# fast build rule for target.
gdb_server/fast:
	$(MAKE) -f CMakeFiles/gdb_server.dir/build.make CMakeFiles/gdb_server.dir/build
.PHONY : gdb_server/fast

#=============================================================================
# Target rules for targets named run_client

# Build rule for target.
run_client: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 run_client
.PHONY : run_client

# fast build rule for target.
run_client/fast:
	$(MAKE) -f CMakeFiles/run_client.dir/build.make CMakeFiles/run_client.dir/build
.PHONY : run_client/fast

#=============================================================================
# Target rules for targets named run_server

# Build rule for target.
run_server: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 run_server
.PHONY : run_server

# fast build rule for target.
run_server/fast:
	$(MAKE) -f CMakeFiles/run_server.dir/build.make CMakeFiles/run_server.dir/build
.PHONY : run_server/fast

# target to build an object file
src/cache.o:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/cache.o
.PHONY : src/cache.o

# target to preprocess a source file
src/cache.i:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/cache.i
.PHONY : src/cache.i

# target to generate assembly for a file
src/cache.s:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/cache.s
.PHONY : src/cache.s

# target to build an object file
src/cache_tests.o:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/cache_tests.o
.PHONY : src/cache_tests.o

# target to preprocess a source file
src/cache_tests.i:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/cache_tests.i
.PHONY : src/cache_tests.i

# target to generate assembly for a file
src/cache_tests.s:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/cache_tests.s
.PHONY : src/cache_tests.s

# target to build an object file
src/dbLL.o:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/dbLL.o
.PHONY : src/dbLL.o

# target to preprocess a source file
src/dbLL.i:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/dbLL.i
.PHONY : src/dbLL.i

# target to generate assembly for a file
src/dbLL.s:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/dbLL.s
.PHONY : src/dbLL.s

# target to build an object file
src/dbLL_tests.o:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/dbLL_tests.o
.PHONY : src/dbLL_tests.o

# target to preprocess a source file
src/dbLL_tests.i:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/dbLL_tests.i
.PHONY : src/dbLL_tests.i

# target to generate assembly for a file
src/dbLL_tests.s:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/dbLL_tests.s
.PHONY : src/dbLL_tests.s

# target to build an object file
src/evict.o:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/evict.o
.PHONY : src/evict.o

# target to preprocess a source file
src/evict.i:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/evict.i
.PHONY : src/evict.i

# target to generate assembly for a file
src/evict.s:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/evict.s
.PHONY : src/evict.s

# target to build an object file
src/evict_tests.o:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/evict_tests.o
.PHONY : src/evict_tests.o

# target to preprocess a source file
src/evict_tests.i:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/evict_tests.i
.PHONY : src/evict_tests.i

# target to generate assembly for a file
src/evict_tests.s:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/evict_tests.s
.PHONY : src/evict_tests.s

# target to build an object file
src/main.o:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/main.o
.PHONY : src/main.o

# target to preprocess a source file
src/main.i:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/main.i
.PHONY : src/main.i

# target to generate assembly for a file
src/main.s:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/main.s
.PHONY : src/main.s

# target to build an object file
src/network_tests.o:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/network_tests.o
.PHONY : src/network_tests.o

# target to preprocess a source file
src/network_tests.i:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/network_tests.i
.PHONY : src/network_tests.i

# target to generate assembly for a file
src/network_tests.s:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/network_tests.s
.PHONY : src/network_tests.s

# target to build an object file
src/node.o:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/node.o
.PHONY : src/node.o

# target to preprocess a source file
src/node.i:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/node.i
.PHONY : src/node.i

# target to generate assembly for a file
src/node.s:
	$(MAKE) -f CMakeFiles/a_networked_cache.dir/build.make CMakeFiles/a_networked_cache.dir/src/node.s
.PHONY : src/node.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... a_networked_cache"
	@echo "... edit_cache"
	@echo "... gdb_client"
	@echo "... gdb_server"
	@echo "... rebuild_cache"
	@echo "... run_client"
	@echo "... run_server"
	@echo "... src/cache.o"
	@echo "... src/cache.i"
	@echo "... src/cache.s"
	@echo "... src/cache_tests.o"
	@echo "... src/cache_tests.i"
	@echo "... src/cache_tests.s"
	@echo "... src/dbLL.o"
	@echo "... src/dbLL.i"
	@echo "... src/dbLL.s"
	@echo "... src/dbLL_tests.o"
	@echo "... src/dbLL_tests.i"
	@echo "... src/dbLL_tests.s"
	@echo "... src/evict.o"
	@echo "... src/evict.i"
	@echo "... src/evict.s"
	@echo "... src/evict_tests.o"
	@echo "... src/evict_tests.i"
	@echo "... src/evict_tests.s"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
	@echo "... src/network_tests.o"
	@echo "... src/network_tests.i"
	@echo "... src/network_tests.s"
	@echo "... src/node.o"
	@echo "... src/node.i"
	@echo "... src/node.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

