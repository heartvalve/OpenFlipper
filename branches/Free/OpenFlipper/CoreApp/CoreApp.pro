################################################################################
#
################################################################################

include( $$TOPDIR/qmake/all.include )

Application()

acg()
openmesh()
glut()
glew()

DIRECTORIES =   ../ ../Core ../Logging \
			    ../Scripting  ../Scripting/scriptPrototypes ../Scripting/scriptWrappers ../SimpleOpt \
			    ../widgets/addEmptyWidget ../widgets/coreWidget ../widgets/helpBrowser \
			    ../widgets/loadWidget ../widgets/optionsWidget ../widgets/unloadPluginsWidget \
			    ../widgets/viewModeWidget

QMAKE_LIBDIR +=  $${TOPDIR}/OpenFlipper/PluginLib/lib/$${BUILDDIRECTORY}

unix {
	LIBS+= -Wl,-rpath=$${TOPDIR}/OpenFlipper/PluginLib/lib/$${BUILDDIRECTORY} -lPluginLib
} 

win32 {
	LIBS+= -L$${TOPDIR}/OpenFlipper/PluginLib/lib/$${BUILDDIRECTORY} -lPluginLib
}

TARGET=OpenFlipper
DESTDIR=$${TOPDIR}/OpenFlipper/$${BUILDDIRECTORY}/

# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS     += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
