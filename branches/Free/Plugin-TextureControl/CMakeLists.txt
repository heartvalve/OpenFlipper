include(plugin)

if (NOT APPLE)
  find_package(Qwt5)
endif ()

if(NOT APPLE AND Qwt5_Qt4_FOUND)
  add_definitions(-DWITH_QWT)
  MESSAGE(STATUS, "include path: ${Qwt5_INCLUDE_DIR}")
  openflipper_plugin(INCDIRS ${Qwt5_INCLUDE_DIR} LIBRARIES ${Qwt5_Qt4_LIBRARY})
else()
  openflipper_plugin()
endif()
