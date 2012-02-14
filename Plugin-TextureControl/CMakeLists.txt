include(plugin)

find_package(Qwt5)

if(Qwt5_Qt4_FOUND)
  message("a")
  add_definitions(-DWITH_QWT)
  openflipper_plugin(INCDIRS ${Qwt5_INCLUDE_DIR} LIBRARIES ${Qwt5_Qt4_LIBRARY})
else()
  openflipper_plugin()
endif()
