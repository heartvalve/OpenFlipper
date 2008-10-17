################################################################################
#
################################################################################

include( $$TOPDIR/qmake/all.include )

Library()

openmesh()
acg()
glut()
glew()
openmp()

DIRECTORIES = ../BasePlugin ../ACGHelper ../common ../common/bsp  ../INIFile $$files( $${TOPDIR}/ObjectTypes/* )
win32 {
	DESTDIR = ../$${BUILDDIRECTORY}
} 

  

# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS   += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
