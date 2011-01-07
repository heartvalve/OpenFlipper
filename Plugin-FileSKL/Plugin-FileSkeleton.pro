################################################################################
# Plugin FileSkeleton
################################################################################

include( $$TOPDIR/qmake/all.include )

Plugin()
mpi()
petsc()
slepc()
gmm()

DEPENDPATH  += $${TOPDIR}/ObjectTypes/Skeleton/ 
INCLUDEPATH += $${TOPDIR}/ObjectTypes/Skeleton/ 
DIRECTORIES = . 


# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS   += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
