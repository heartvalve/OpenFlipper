################################################################################
#
################################################################################

include( $$TOPDIR/qmake/all.include )

Plugin()

DIRECTORIES = .


exists(  $$TOPDIR/ObjectTypes/BSplineCurve) {
  DEFINES *= ENABLE_BSPLINECURVE_SUPPORT
}

exists(  $$TOPDIR/ObjectTypes/BSplineSurface) {
  DEFINES *= ENABLE_BSPLINESURFACE_SUPPORT
}


# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS   += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
