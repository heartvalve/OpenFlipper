include( $$TOPDIR/qmake/all.include )

Subdirs()

unix {
	exists( SensorFramework ) {
		addSubdirs( SensorFramework )
	}
}

addSubdirs( OpenMesh/Core)
addSubdirs( OpenMesh/Tools, OpenMesh/Core  )
addSubdirs( ACG , OpenMesh/Core )

unix {
	exists(PhySim) {
            addSubdirs( PhySim, ACG )
        }
}

addSubdirs( OpenFlipper/PluginLib , ACG OpenMesh/Core OpenMesh/Tools )
addSubdirs( OpenFlipper/CoreApp , ACG OpenFlipper/PluginLib OpenMesh/Core OpenMesh/Tools )

addSubdirs( Plugin-*, OpenFlipper/PluginLib OpenMesh/Core  OpenMesh/Tools ACG   )

macx {
	SUBDIRS -= Plugin_SpaceNavigator
}

# ugly hack to call qmake separately in each plugin subdir. Otherwise it segfaults after 20 Plugins or so
# when using recursive Deprecated since qmake from qt4.5
#win32 {
#	!ReleaseBuild{
#		!DebugBuild {
#			MAKECMD = %QTDIR%\qmake.exe
#
 #			CORELIST += OpenMesh/Core OpenMesh/Tools ACG  OpenFlipper/PluginLib OpenFlipper/CoreApp 
#			for(entry, CORELIST ): {
#				message( $$entry )
#				system( cd $$entry && $$MAKECMD  )			
#			}
#
#			PLUGINLIST = $$system( dir /b Plugin-* )
#			for(entry, PLUGINLIST ): {
#				message( $$entry )
#				exists($${entry}/$${entry}.pro) {
#					system( cd $$entry && $$MAKECMD  )			
#				}
#
#			}
#		}
#	}
#	SUBDIRS -= Plugin_SpaceNavigator
#	
#}

