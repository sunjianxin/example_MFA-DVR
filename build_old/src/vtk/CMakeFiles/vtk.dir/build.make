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
include src/vtk/CMakeFiles/vtk.dir/depend.make

# Include the progress variables for this target.
include src/vtk/CMakeFiles/vtk.dir/progress.make

# Include the compile flags for this target's objects.
include src/vtk/CMakeFiles/vtk.dir/flags.make

src/vtk/CMakeFiles/vtk.dir/vtk.cpp.o: src/vtk/CMakeFiles/vtk.dir/flags.make
src/vtk/CMakeFiles/vtk.dir/vtk.cpp.o: ../src/vtk/vtk.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/jianxinsun/research/intern/github/vis-mfa/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/vtk/CMakeFiles/vtk.dir/vtk.cpp.o"
	cd /Users/jianxinsun/research/intern/github/vis-mfa/build/src/vtk && /usr/local/bin/mpicxx  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/vtk.dir/vtk.cpp.o -c /Users/jianxinsun/research/intern/github/vis-mfa/src/vtk/vtk.cpp

src/vtk/CMakeFiles/vtk.dir/vtk.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vtk.dir/vtk.cpp.i"
	cd /Users/jianxinsun/research/intern/github/vis-mfa/build/src/vtk && /usr/local/bin/mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/jianxinsun/research/intern/github/vis-mfa/src/vtk/vtk.cpp > CMakeFiles/vtk.dir/vtk.cpp.i

src/vtk/CMakeFiles/vtk.dir/vtk.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vtk.dir/vtk.cpp.s"
	cd /Users/jianxinsun/research/intern/github/vis-mfa/build/src/vtk && /usr/local/bin/mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/jianxinsun/research/intern/github/vis-mfa/src/vtk/vtk.cpp -o CMakeFiles/vtk.dir/vtk.cpp.s

# Object files for target vtk
vtk_OBJECTS = \
"CMakeFiles/vtk.dir/vtk.cpp.o"

# External object files for target vtk
vtk_EXTERNAL_OBJECTS =

src/vtk/vtk: src/vtk/CMakeFiles/vtk.dir/vtk.cpp.o
src/vtk/vtk: src/vtk/CMakeFiles/vtk.dir/build.make
src/vtk/vtk: /usr/local/lib/libvtkIOImage-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkInteractionStyle-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkRenderingAnnotation-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkRenderingContextOpenGL2-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkRenderingGL2PSOpenGL2-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkRenderingVolumeOpenGL2-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/Cellar/open-mpi/4.1.2/lib/libmpi.dylib
src/vtk/vtk: /usr/local/lib/libvtkIOLegacy-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkIOCore-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkRenderingContext2D-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkRenderingFreeType-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkfreetype-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkzlib-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkRenderingOpenGL2-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkRenderingUI-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkRenderingVolume-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkRenderingCore-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkCommonColor-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkFiltersSources-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkFiltersGeneral-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkFiltersCore-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkImagingCore-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkglew-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkImagingMath-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkCommonExecutionModel-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkCommonDataModel-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkCommonTransforms-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkCommonMisc-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkCommonMath-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtkCommonCore-9.0.9.0.3.dylib
src/vtk/vtk: /usr/local/lib/libvtksys-9.0.9.0.3.dylib
src/vtk/vtk: src/vtk/CMakeFiles/vtk.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/jianxinsun/research/intern/github/vis-mfa/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable vtk"
	cd /Users/jianxinsun/research/intern/github/vis-mfa/build/src/vtk && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vtk.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/vtk/CMakeFiles/vtk.dir/build: src/vtk/vtk

.PHONY : src/vtk/CMakeFiles/vtk.dir/build

src/vtk/CMakeFiles/vtk.dir/clean:
	cd /Users/jianxinsun/research/intern/github/vis-mfa/build/src/vtk && $(CMAKE_COMMAND) -P CMakeFiles/vtk.dir/cmake_clean.cmake
.PHONY : src/vtk/CMakeFiles/vtk.dir/clean

src/vtk/CMakeFiles/vtk.dir/depend:
	cd /Users/jianxinsun/research/intern/github/vis-mfa/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/jianxinsun/research/intern/github/vis-mfa /Users/jianxinsun/research/intern/github/vis-mfa/src/vtk /Users/jianxinsun/research/intern/github/vis-mfa/build /Users/jianxinsun/research/intern/github/vis-mfa/build/src/vtk /Users/jianxinsun/research/intern/github/vis-mfa/build/src/vtk/CMakeFiles/vtk.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/vtk/CMakeFiles/vtk.dir/depend

