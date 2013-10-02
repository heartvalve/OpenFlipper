include(plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyhedralMesh)
    add_definitions (-DENABLE_OPENVOLUMEMESH_SUPPORT)
    add_definitions (-DENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT)
endif()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/HexahedralMesh)
    add_definitions (-DENABLE_OPENVOLUMEMESH_SUPPORT)
    add_definitions (-DENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT)
endif()

# use Qwt6 for QT5
if (NOT FORCE_QT4 AND QT5_FOUND)
  find_package(Qwt6)

  if (QWT6_FOUND)
    add_definitions(-DWITH_QWT)
    openflipper_plugin(INCDIRS ${QWT6_INCLUDE_DIRS} LIBRARIES ${QWT6_LIBRARIES})
  else()
    openflipper_plugin()
  endif()
else()
  find_package(Qwt5)

  if(Qwt5_Qt4_FOUND)
    add_definitions(-DWITH_QWT)
    openflipper_plugin(INCDIRS ${Qwt5_INCLUDE_DIR} LIBRARIES ${Qwt5_Qt4_LIBRARY})
  else()
    openflipper_plugin()
  endif()
endif()
