################################################################################
#
################################################################################

include( $$TOPDIR/qmake/all.include )

Plugin()
Doc(Plugin-Datacontrol)

DIRECTORIES = .

# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS     += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
