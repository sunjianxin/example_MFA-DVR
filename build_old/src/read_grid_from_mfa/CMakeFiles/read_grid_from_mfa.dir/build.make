# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.15.1/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.15.1/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/jianxinsun/research/intern/github/vis-mfa

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/jianxinsun/research/intern/github/vis-mfa/build

# Include any dependencies generated for this target.
include src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/depend.make

# Include the progress variables for this target.
include src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/progress.make

# Include the compile flags for this target's objects.
include src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/flags.make

src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/read_grid_from_mfa.cpp.o: src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/flags.make
src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/read_grid_from_mfa.cpp.o: ../src/read_grid_from_mfa/read_grid_from_mfa.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/jianxinsun/research/intern/github/vis-mfa/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/read_grid_from_mfa.cpp.o"
	cd /Users/jianxinsun/research/intern/github/vis-mfa/build/src/read_grid_from_mfa && /usr/local/bin/mpicxx  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/read_grid_from_mfa.dir/read_grid_from_mfa.cpp.o -c /Users/jianxinsun/research/intern/github/vis-mfa/src/read_grid_from_mfa/read_grid_from_mfa.cpp

src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/read_grid_from_mfa.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/read_grid_from_mfa.dir/read_grid_from_mfa.cpp.i"
	cd /Users/jianxinsun/research/intern/github/vis-mfa/build/src/read_grid_from_mfa && /usr/local/bin/mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/jianxinsun/research/intern/github/vis-mfa/src/read_grid_from_mfa/read_grid_from_mfa.cpp > CMakeFiles/read_grid_from_mfa.dir/read_grid_from_mfa.cpp.i

src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/read_grid_from_mfa.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/read_grid_from_mfa.dir/read_grid_from_mfa.cpp.s"
	cd /Users/jianxinsun/research/intern/github/vis-mfa/build/src/read_grid_from_mfa && /usr/local/bin/mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/jianxinsun/research/intern/github/vis-mfa/src/read_grid_from_mfa/read_grid_from_mfa.cpp -o CMakeFiles/read_grid_from_mfa.dir/read_grid_from_mfa.cpp.s

# Object files for target read_grid_from_mfa
read_grid_from_mfa_OBJECTS = \
"CMakeFiles/read_grid_from_mfa.dir/read_grid_from_mfa.cpp.o"

# External object files for target read_grid_from_mfa
read_grid_from_mfa_EXTERNAL_OBJECTS =

src/read_grid_from_mfa/read_grid_from_mfa: src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/read_grid_from_mfa.cpp.o
src/read_grid_from_mfa/read_grid_from_mfa: src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/build.make
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkIOImage-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkInteractionStyle-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkRenderingAnnotation-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkRenderingContextOpenGL2-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkRenderingGL2PSOpenGL2-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkRenderingVolumeOpenGL2-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/Cellar/open-mpi/4.1.2/lib/libmpi.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkIOLegacy-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkIOCore-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkRenderingContext2D-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkRenderingFreeType-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkfreetype-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkzlib-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkRenderingOpenGL2-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkRenderingUI-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkRenderingVolume-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkRenderingCore-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkCommonColor-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkFiltersSources-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkFiltersGeneral-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkFiltersCore-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkImagingCore-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkglew-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkImagingMath-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkCommonExecutionModel-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkCommonDataModel-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkCommonTransforms-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkCommonMisc-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkCommonMath-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtkCommonCore-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: /usr/local/lib/libvtksys-9.0.9.0.3.dylib
src/read_grid_from_mfa/read_grid_from_mfa: src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/jianxinsun/research/intern/github/vis-mfa/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable read_grid_from_mfa"
	cd /Users/jianxinsun/research/intern/github/vis-mfa/build/src/read_grid_from_mfa && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/read_grid_from_mfa.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/build: src/read_grid_from_mfa/read_grid_from_mfa

.PHONY : src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/build

src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/clean:
	cd /Users/jianxinsun/research/intern/github/vis-mfa/build/src/read_grid_from_mfa && $(CMAKE_COMMAND) -P CMakeFiles/read_grid_from_mfa.dir/cmake_clean.cmake
.PHONY : src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/clean

src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/depend:
	cd /Users/jianxinsun/research/intern/github/vis-mfa/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/jianxinsun/research/intern/github/vis-mfa /Users/jianxinsun/research/intern/github/vis-mfa/src/read_grid_from_mfa /Users/jianxinsun/research/intern/github/vis-mfa/build /Users/jianxinsun/research/intern/github/vis-mfa/build/src/read_grid_from_mfa /Users/jianxinsun/research/intern/github/vis-mfa/build/src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/read_grid_from_mfa/CMakeFiles/read_grid_from_mfa.dir/depend
