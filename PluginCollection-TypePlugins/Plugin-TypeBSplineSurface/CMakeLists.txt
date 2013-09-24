include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/BSplineSurface/BSplineSurface.hh)
  openflipper_plugin (INSTALLDATA Icons)
else()
  message( WARNING "Plugin TypeBSplineSurface disabled due to missing BSplineSurface ObjectType" )
endif ()