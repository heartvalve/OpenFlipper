include(plugin)

find_package(Qwt5)

if(Qwt5_Qt4_FOUND)
  add_definitions(-DWITH_QWT)
  openflipper_plugin(INCDIRS ${Qwt5_INCLUDE_DIR} LIBRARIES ${Qwt5_Qt4_LIBRARY})
else()
  openflipper_plugin()
endif()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyhedralMesh)
    add_definitions (-DENABLE_OPENVOLUMEMESH_SUPPORT)
    add_definitions (-DENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT)
endif()

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/HexahedralMesh)
    add_definitions (-DENABLE_OPENVOLUMEMESH_SUPPORT)
    add_definitions (-DENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT)
endif()
