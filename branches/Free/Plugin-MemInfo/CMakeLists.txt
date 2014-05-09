include (plugin)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_subdirectory(procps)
	openflipper_plugin (
		LIBRARIES procps
		CDEFINITIONSADD PROCPS_ENABLED)
ELSE()
	openflipper_plugin ()
ENDIF()
