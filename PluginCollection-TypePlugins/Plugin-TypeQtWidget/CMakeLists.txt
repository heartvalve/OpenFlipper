include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/QtWidget/QtWidget.hh)
  openflipper_plugin (INSTALLDATA Icons)
else()
  message( WARNING "Plugin TypeQtWidget disabled due to missing QtWidget ObjectType" )
endif ()

