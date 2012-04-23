include (plugin)
openflipper_plugin (INCDIRS ${CMAKE_BINARY_DIR}/ACG 
                    TRANSLATION_LANGUAGES de_DE 
                    INSTALLDATA Icons )
add_dependencies(Plugin-Datacontrol ACG OpenFlipper)
