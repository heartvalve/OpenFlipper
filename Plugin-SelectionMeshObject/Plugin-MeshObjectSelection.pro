################################################################################
#
################################################################################

include( $$TOPDIR/qmake/all.include )

Plugin()
openmp()

DIRECTORIES = widgets


# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS   += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
