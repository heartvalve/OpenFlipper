################################################################################
#
################################################################################

include( $$TOPDIR/qmake/all.include )

# For the help system
CONFIG += help

Application()

acg()
openmesh()
glut()
glew()
openmp()
qt()

DIRECTORIES =   ../ ../Core ../Logging \
			    ../Scripting  ../Scripting/scriptPrototypes ../Scripting/scriptWrappers ../SimpleOpt \
			    ../widgets/aboutWidget ../widgets/addEmptyWidget ../widgets/loggerWidget \
                            ../widgets/coreWidget ../widgets/helpWidget \
			    ../widgets/loadWidget ../widgets/optionsWidget ../widgets/PluginDialog \
			    ../widgets/viewModeWidget \
			    ../widgets/videoCaptureDialog ../widgets/snapshotDialog

QMAKE_LIBDIR +=  $${TOPDIR}/OpenFlipper/PluginLib/lib/$${BUILDDIRECTORY}

unix {
	!contains( OPERATING_SYSTEM, Darwin ) {
		LIBS += -Wl,-rpath=$${TOPDIR}/OpenFlipper/PluginLib/lib/$${BUILDDIRECTORY} -lPluginLib
	}	
}

macx {
        LIBS += -L$${TOPDIR}/OpenFlipper/PluginLib/lib/$${BUILDDIRECTORY} -lPluginLib
        QMAKE_LFLAGS_SONAME += -install_name$${LITERAL_WHITESPACE}$${TOPDIR}/OpenFlipper/PluginLib/lib/$${BUILDDIRECTORY}/
	ICON = $$TOPDIR/OpenFlipper/Icons/OpenFlipper_Icon.icns
}

win32 {
	LIBS+= -L$${TOPDIR}/OpenFlipper/$${BUILDDIRECTORY} -lPluginLib
}

TARGET=OpenFlipper
DESTDIR=$${TOPDIR}/OpenFlipper/$${BUILDDIRECTORY}/

# Input
HEADERS += $$getFilesFromDir($$DIRECTORIES,*.hh)
SOURCES += $$getFilesFromDir($$DIRECTORIES,*.cc)
FORMS   += $$getFilesFromDir($$DIRECTORIES,*.ui)

################################################################################
