include(plugin)

# use Qwt6 for QT5
if (NOT FORCE_QT4 AND QT5_FOUND)
  find_package(Qwt6)

  if (QWT6_FOUND)
    add_definitions(-DWITH_QWT)
    openflipper_plugin(INCDIRS ${QWT6_INCLUDE_DIRS}
      LIBRARIES ${QWT6_LIBRARIES}
      INSTALLDATA Icons
    )
  else()
    openflipper_plugin(INSTALLDATA Icons)
  endif()
else()
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
endif()
