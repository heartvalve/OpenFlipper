include( $$TOPDIR/qmake/all.include )

Subdirs()

unix {
	exists( SensorFramework ) {
		addSubdirs( SensorFramework )
	}
}

addSubdirs( OpenMesh/Core)
addSubdirs( ACG , OpenMesh/Core )
addSubdirs( OpenMesh/Tools, OpenMesh/Core  )
addSubdirs( OpenFlipper/PluginLib , ACG OpenMesh/Core OpenMesh/Tools )
addSubdirs( OpenFlipper/CoreApp , ACG OpenFlipper/PluginLib OpenMesh/Core OpenMesh/Tools )

addSubdirs( Plugin-*, OpenFlipper/PluginLib OpenMesh/Core  OpenMesh/Tools ACG   )

# ugly hack to call qmake separately in each plugin subdir. Otherwise it segfaults after 20 Plugins or so
# when using recursive
win32 {
	!ReleaseBuild{
		!DebugBuild {
			MAKECMD = c:\qt4\bin\qmake.exe

 			CORELIST += OpenMesh/Core OpenMesh/Tools ACG  OpenFlipper/PluginLib OpenFlipper/CoreApp 
			for(entry, CORELIST ): {
				message( $$entry )
				system( cd $$entry && $$MAKECMD  )			
			}

			PLUGINLIST = $$system( dir /b Plugin-* )
			for(entry, PLUGINLIST ): {
				message( $$entry )
				exists($${entry}/$${entry}.pro) {
					system( cd $$entry && $$MAKECMD  )			
				}

			}
		}
	}
	SUBDIRS -= Plugin_AachenMap
	SUBDIRS -= Plugin_SpaceNavigator
	SUBDIRS -= Plugin_ToradexSensors
	SUBDIRS -= Plugin_ReverseEngineering
	SUBDIRS -= Plugin_FeatureDetection
	SUBDIRS -= Plugin_GeometryTransfer
	SUBDIRS -= Plugin_Hodge
	SUBDIRS -= Plugin_PrincipalCurvature
	SUBDIRS -= Plugin_QuadCover
	SUBDIRS -= Plugin_Curve
	SUBDIRS -= Plugin_Graphs

	message( Todo : Plugins Plugin_AachenMap Plugin_SpaceNavigator Plugin_ToradexSensors Plugin_ReverseEngineering Plugin_FeatureDetection Plugin_GeometryTransfer Plugin_Hodge Plugin_PrincipalCurvature Plugin_TemplateModeling Plugin_QuadCover Plugin_Curve Plugin_Graphs)
}

