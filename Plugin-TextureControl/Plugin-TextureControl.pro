################################################################################
#
################################################################################

include( $$TOPDIR/qmake/all.include )

qwt() {
   DEFINES+=WITH_QWT
}	

Plugin()

DIRECTORIES = .

# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS     += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
