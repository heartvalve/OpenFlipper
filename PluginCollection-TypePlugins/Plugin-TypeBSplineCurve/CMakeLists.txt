include (plugin)

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/BSplineCurve/BSplineCurve.hh)
  openflipper_plugin (INSTALLDATA Icons)
else()
  message( WARNING "Plugin TypeBSplineCurve disabled due to missing BSplineCurve ObjectType" )
endif ()
