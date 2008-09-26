################################################################################
#
################################################################################

#include( $$TOPDIR/qmake/all.include )

Library()

acg()
openmesh()
glut()
glew()

DIRECTORIES = ../BasePlugin ../common ../common/bsp  ../INIFile 

# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS     += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
