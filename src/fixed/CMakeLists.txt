include_directories         (${CMAKE_SOURCE_DIR}/src)

if (NOT VTK_FOUND)
  message("Skipping CylinderExample: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")
if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  # add_executable(CylinderExample MACOSX_BUNDLE CylinderExample.cxx )
  add_executable(fixed fixed.cpp)
  target_link_libraries(fixed PRIVATE ${VTK_LIBRARIES} ${libraries})
else ()
  # include all components
  # add_executable(CylinderExample MACOSX_BUNDLE CylinderExample.cxx )
  add_executable(fixed fixed.cpp )
  target_link_libraries(fixed PRIVATE ${VTK_LIBRARIES} ${libraries})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS fixed
    MODULES ${VTK_LIBRARIES}
    )
endif ()

# add_executable              (vtk_volume                       vtk_volume.cpp)

# target_link_libraries       (vtk_volume                       ${libraries})

set_property                (TARGET fixed                PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)

install                     (TARGETS fixed
                            DESTINATION ${CMAKE_INSTALL_PREFIX}/src/fixed/
                            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                            GROUP_READ GROUP_WRITE GROUP_EXECUTE
                            WORLD_READ WORLD_WRITE WORLD_EXECUTE)





