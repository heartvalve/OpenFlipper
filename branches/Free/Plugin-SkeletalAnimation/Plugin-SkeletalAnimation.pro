################################################################################
# Plugin SkeletalAnimation
################################################################################

include( $${TOPDIR}/qmake/all.include )

Plugin()

DIRECTORIES = . 
DIRECTORIES += $$files($${TOPDIR}/Plugin-SkeletalAnimation/*)

# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh) 
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc) 
FORMS   += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
