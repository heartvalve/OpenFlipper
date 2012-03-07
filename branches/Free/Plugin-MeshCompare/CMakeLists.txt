include(plugin)

find_package(Qwt5)

if(Qwt5_Qt4_FOUND)
  add_definitions(-DWITH_QWT)
  openflipper_plugin(INCDIRS ${Qwt5_INCLUDE_DIR} 
                     LIBRARIES ${Qwt5_Qt4_LIBRARY}
                     INSTALLDATA Icons
                     )
else()
  openflipper_plugin(INSTALLDATA Icons)
endif()
