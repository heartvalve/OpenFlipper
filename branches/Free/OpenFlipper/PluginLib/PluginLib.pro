################################################################################
#
################################################################################

include( $$TOPDIR/qmake/all.include )

Library()

acg()
openmesh()
glut()
glew()
openmp()

DEPENDPATH += $$files( $${TOPDIR}/ObjectTypes/* )
DIRECTORIES = ../BasePlugin ../ACGHelper ../common ../common/bsp  ../INIFile $$files( $${TOPDIR}/ObjectTypes/* )

# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS   += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
