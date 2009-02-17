################################################################################
#
################################################################################

include( $$TOPDIR/qmake/all.include )

Plugin()
openmp()

exists(  $$TOPDIR/ObjectTypes/PolyLine) {
  DEFINES *= ENABLE_POLYLINE_SUPPORT
}

DIRECTORIES = . PolyLine BSplineCurve widgets


# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS     += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
