################################################################################
# QMake project file for SimplePlugin
################################################################################

include( $$TOPDIR/qmake/all.include )

Plugin()

DIRECTORIES = .

# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS   += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################

