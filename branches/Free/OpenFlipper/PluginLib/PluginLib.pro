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

DIRECTORIES = ../BasePlugin ../ACGHelper ../common ../common/bsp  ../INIFile $$files( $${TOPDIR}/ObjectTypes/* ) \
              ../widgets/glWidget
win32 {
	DESTDIR = ../$${BUILDDIRECTORY}
	DEFINES += PLUGINLIBDLL
} 

  

# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS   += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
