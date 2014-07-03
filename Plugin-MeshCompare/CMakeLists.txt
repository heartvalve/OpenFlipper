include(plugin)

# use Qwt6 for QT5
if (NOT FORCE_QT4 AND QT5_FOUND)
  find_package(Qwt6)

  if (QWT6_FOUND)
    add_definitions(-DWITH_QWT)
    openflipper_plugin(DEPS Qwt6
      INSTALLDATA Icons
    )
  else()
    openflipper_plugin(INSTALLDATA Icons)
  endif()
else()
  find_package(Qwt5)

  if(Qwt5_Qt4_FOUND)
    add_definitions(-DWITH_QWT)
    openflipper_plugin(DEPS Qwt5
      INSTALLDATA Icons
    )
  else()
    openflipper_plugin(INSTALLDATA Icons)
  endif()
endif()
