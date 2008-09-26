################################################################################
#
################################################################################

#include( $$TOPDIR/qmake/all.include )

Application()

acg()
openmesh()
glut()
glew()

DIRECTORIES = . ACGHelper BasePlugin common common/bsp Core INIFile Logging \
			    Scripting  Scripting/scriptPrototypes Scripting/scriptWrappers SimpleOpt \
			    widgets/addEmptyWidget widgets/coreWidget widgets/helpBrowser \
			    widgets/loadWidget widgets/optionsWidget widgets/unloadPluginsWidget \
			    widgets/viewModeWidget

message($$DIRECTORIES)

# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS     += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
