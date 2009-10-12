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
ftgl()

DIRECTORIES = ../BasePlugin ../ACGHelper ../common ../common/bsp  ../INIFile $$files( $${TOPDIR}/ObjectTypes/* ) \
              ../widgets/glWidget \
	      ../threads
	      
win32 {
	DESTDIR = ../$${BUILDDIRECTORY}
	DEFINES += PLUGINLIBDLL
} 

macx {
  QMAKE_LFLAGS_SONAME = -install_name$${LITERAL_WHITESPACE}$${TOPDIR}/OpenFlipper/PluginLib/lib/$${BUILDDIRECTORY}/
  export( QMAKE_LFLAGS_SONAME )
}  

# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS   += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
