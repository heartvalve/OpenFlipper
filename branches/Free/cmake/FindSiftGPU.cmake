# - Try to find SiftGPU
# Once done this will define
#  
# SIFTGPU_FOUND             - system has UTIL
# SIFTGPU_INCLUDE_DIR       - the UTIL include directory
# SIFTGPU_LIBRARY           - Link these to use UTIL
# SIFTGPU_LIBRARY_DIR       - Library DIR of UTIL
#   

IF (SIFTGPU_INCLUDE_DIR)
	# Already in cache, be silent
	SET(SIFT_GPU_FIND_QUIETLY TRUE)
ENDIF (SIFTGPU_INCLUDE_DIR)

if( WIN32 )
	# TODO: find out which directory to reference here
	set( LIBDIR lib ) 
elseif( UNIX )
	set( LIBDIR bin )
elseif( APPLE )
	set( LIBDIR bin )
endif()


FIND_PATH( SIFTGPU_INCLUDE_DIR SiftGPU/SiftGPU.h PATHS "${CMAKE_SOURCE_DIR}/../SiftGPU/src" )


if( SIFTGPU_INCLUDE_DIR )
	set( SIFTGPU_FOUND TRUE )
	set( SIFTGPU_LIBRARY_DIR ${SIFTGPU_INCLUDE_DIR}/../${LIBDIR} )
	set( SIFTGPU_LIBRARY optimized siftgpu debug siftgpu )
ELSE( SIFTGPU_INCLUDE_DIR )
	SET( SIFTGPU_FOUND FALSE )
ENDIF( SIFTGPU_INCLUDE_DIR )

IF ( SIFTGPU_FOUND )
ELSE ( SIFTGPU_FOUND )
	# MESSAGE( WARNING "Warning: SiftGPU NOT FOUND" ) # Warning is given by OpenFlipper
ENDIF ( SIFTGPU_FOUND )
