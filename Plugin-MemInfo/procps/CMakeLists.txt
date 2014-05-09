add_library(procps SHARED version.c sysinfo.c alloc.c)
set_target_properties(procps PROPERTIES
	COMPILE_DEFINITIONS "PACKAGE_NAME=\"OpenFlipper\";PACKAGE_VERSION=\"3.3.3\";_XOPEN_SOURCE=700"
	COMPILE_FLAGS "-std=c99")
