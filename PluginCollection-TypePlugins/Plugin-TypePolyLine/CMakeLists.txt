include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyLine/PolyLine.hh)
  openflipper_plugin (INSTALLDATA Icons)
else()
  message( WARNING "Plugin TypePolyLine disabled due to missing PolyLine ObjectType" )
endif ()

