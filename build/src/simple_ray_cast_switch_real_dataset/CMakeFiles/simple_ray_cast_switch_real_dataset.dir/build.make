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
CMAKE_SOURCE_DIR = /Users/jianxinsun/research/intern/github/example_MFA-DVR

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/jianxinsun/research/intern/github/example_MFA-DVR/build

# Include any dependencies generated for this target.
include src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/depend.make

# Include the progress variables for this target.
include src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/progress.make

# Include the compile flags for this target's objects.
include src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/flags.make

src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/simple_ray_cast_switch_real_dataset.cpp.o: src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/flags.make
src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/simple_ray_cast_switch_real_dataset.cpp.o: ../src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/jianxinsun/research/intern/github/example_MFA-DVR/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/simple_ray_cast_switch_real_dataset.cpp.o"
	cd /Users/jianxinsun/research/intern/github/example_MFA-DVR/build/src/simple_ray_cast_switch_real_dataset && /usr/local/bin/mpicxx  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/simple_ray_cast_switch_real_dataset.dir/simple_ray_cast_switch_real_dataset.cpp.o -c /Users/jianxinsun/research/intern/github/example_MFA-DVR/src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset.cpp

src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/simple_ray_cast_switch_real_dataset.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/simple_ray_cast_switch_real_dataset.dir/simple_ray_cast_switch_real_dataset.cpp.i"
	cd /Users/jianxinsun/research/intern/github/example_MFA-DVR/build/src/simple_ray_cast_switch_real_dataset && /usr/local/bin/mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/jianxinsun/research/intern/github/example_MFA-DVR/src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset.cpp > CMakeFiles/simple_ray_cast_switch_real_dataset.dir/simple_ray_cast_switch_real_dataset.cpp.i

src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/simple_ray_cast_switch_real_dataset.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/simple_ray_cast_switch_real_dataset.dir/simple_ray_cast_switch_real_dataset.cpp.s"
	cd /Users/jianxinsun/research/intern/github/example_MFA-DVR/build/src/simple_ray_cast_switch_real_dataset && /usr/local/bin/mpicxx $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/jianxinsun/research/intern/github/example_MFA-DVR/src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset.cpp -o CMakeFiles/simple_ray_cast_switch_real_dataset.dir/simple_ray_cast_switch_real_dataset.cpp.s

# Object files for target simple_ray_cast_switch_real_dataset
simple_ray_cast_switch_real_dataset_OBJECTS = \
"CMakeFiles/simple_ray_cast_switch_real_dataset.dir/simple_ray_cast_switch_real_dataset.cpp.o"

# External object files for target simple_ray_cast_switch_real_dataset
simple_ray_cast_switch_real_dataset_EXTERNAL_OBJECTS =

src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/simple_ray_cast_switch_real_dataset.cpp.o
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/build.make
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkIOImage-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkInteractionStyle-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkRenderingAnnotation-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkRenderingContextOpenGL2-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkRenderingGL2PSOpenGL2-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkRenderingVolumeOpenGL2-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/Cellar/open-mpi/4.1.2/lib/libmpi.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkIOLegacy-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkIOCore-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkRenderingContext2D-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkRenderingFreeType-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkfreetype-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkzlib-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkRenderingOpenGL2-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkRenderingUI-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkRenderingVolume-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkRenderingCore-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkCommonColor-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkFiltersSources-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkFiltersGeneral-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkFiltersCore-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkImagingCore-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkglew-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkImagingMath-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkCommonExecutionModel-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkCommonDataModel-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkCommonTransforms-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkCommonMisc-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkCommonMath-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtkCommonCore-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: /usr/local/lib/libvtksys-9.0.9.0.3.dylib
src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset: src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/jianxinsun/research/intern/github/example_MFA-DVR/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable simple_ray_cast_switch_real_dataset"
	cd /Users/jianxinsun/research/intern/github/example_MFA-DVR/build/src/simple_ray_cast_switch_real_dataset && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/simple_ray_cast_switch_real_dataset.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/build: src/simple_ray_cast_switch_real_dataset/simple_ray_cast_switch_real_dataset

.PHONY : src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/build

src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/clean:
	cd /Users/jianxinsun/research/intern/github/example_MFA-DVR/build/src/simple_ray_cast_switch_real_dataset && $(CMAKE_COMMAND) -P CMakeFiles/simple_ray_cast_switch_real_dataset.dir/cmake_clean.cmake
.PHONY : src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/clean

src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/depend:
	cd /Users/jianxinsun/research/intern/github/example_MFA-DVR/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/jianxinsun/research/intern/github/example_MFA-DVR /Users/jianxinsun/research/intern/github/example_MFA-DVR/src/simple_ray_cast_switch_real_dataset /Users/jianxinsun/research/intern/github/example_MFA-DVR/build /Users/jianxinsun/research/intern/github/example_MFA-DVR/build/src/simple_ray_cast_switch_real_dataset /Users/jianxinsun/research/intern/github/example_MFA-DVR/build/src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/simple_ray_cast_switch_real_dataset/CMakeFiles/simple_ray_cast_switch_real_dataset.dir/depend
