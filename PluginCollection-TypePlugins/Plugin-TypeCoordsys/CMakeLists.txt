include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/Coordsys/Coordsys.hh)
  openflipper_plugin (INSTALLDATA Icons)
else()
  message( WARNING "Plugin TypeCoordsys disabled due to missing Coordsys ObjectType" )
endif ()
