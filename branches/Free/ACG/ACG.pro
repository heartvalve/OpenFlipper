################################################################################
#
################################################################################

include( $$TOPDIR/qmake/all.include )
Library()
glew()
glut()

openmesh()

contains( OPENFLIPPER , OpenFlipper ){
   ftgl()
   message(ACG Library is Building for OpenFlipper )
}


DIRECTORIES = . Geometry Config/ Geometry/Types GL Glut IO Math QtWidgets Scenegraph QtScenegraph ShaderUtils Utils

win32 {
 DEFINES += _USE_MATH_DEFINES NOMINMAX INCLUDE_TEMPLATES
# CONFIG  += static
 DEFINES += ACGDLL
 contains( OPENFLIPPER , OpenFlipper ){
   DESTDIR = $${TOPDIR}/OpenFlipper/$${BUILDDIRECTORY}
 }
}

macx {
  QMAKE_LFLAGS_SONAME = -install_name$${LITERAL_WHITESPACE}$${TOPDIR}/ACG/lib/$${BUILDDIRECTORY}/
  export( QMAKE_LFLAGS_SONAME )
}

# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS   += $$getFilesFromDir($$DIRECTORIES,*.ui)

installs()

################################################################################
