include( $$TOPDIR/qmake/all.include )

Subdirs()

unix {
	exists( SensorFramework ) {
		addSubdirs( SensorFramework )
	}
}

addSubdirs( OpenMesh/src/OpenMesh/Core)
addSubdirs( OpenMesh/src/OpenMesh/Tools, OpenMesh/src/OpenMesh/Core  )
addSubdirs( ACG , OpenMesh/src/OpenMesh/Core )

unix {
	exists(PhySim) {
            addSubdirs( PhySim, ACG )
        }
}

addSubdirs( OpenFlipper/PluginLib , ACG OpenMesh/src/OpenMesh/Core OpenMesh/src/OpenMesh/Tools )
addSubdirs( OpenFlipper/CoreApp , ACG OpenFlipper/PluginLib OpenMesh/src/OpenMesh/Core OpenMesh/src/OpenMesh/Tools )

addSubdirs( Plugin-*, OpenFlipper/PluginLib OpenMesh/src/OpenMesh/Core  OpenMesh/src/OpenMesh/Tools ACG   )

macx {
	SUBDIRS -= Plugin_SpaceNavigator
}
