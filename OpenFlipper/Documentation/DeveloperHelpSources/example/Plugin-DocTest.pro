################################################################################
#
################################################################################

include( $$TOPDIR/qmake/all.include )

Plugin()

Doc(Plugin-SimplePlugin)

DIRECTORIES = .

# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS   += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
