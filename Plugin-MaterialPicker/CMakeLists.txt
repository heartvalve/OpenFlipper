include (plugin)

openflipper_plugin (INCDIRS ${CMAKE_BINARY_DIR}/ACG 
                    INSTALLDATA Icons)
add_dependencies(Plugin-MaterialPicker ACG OpenFlipper)
