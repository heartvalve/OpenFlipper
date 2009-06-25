include (ACGCommon)

acg_qt4 (REQUIRED 4.5)
acg_openmp ()
acg_ftgl ()

# check for OpenGL, GLEW and GLUT as our required dependencies
find_package (OpenGL)
if (NOT OPENGL_FOUND)
  message (FATAL_ERROR "OpengGL not found!")
endif ()

find_package (GLEW)
if (NOT GLEW_FOUND)
  message (FATAL_ERROR "GLEW not found!")
endif ()

find_package (GLUT)
if (NOT GLUT_FOUND)
  message (FATAL_ERROR "GLUT not found!")
endif ()


find_package( Freetype) 
if (NOT FREETYPE_FOUND)
  message (MESSAGE "FREETYPE not found! Disabled ftgl support!")
else (NOT FREETYPE_FOUND)

  find_package (FTGL)
  if (NOT FTGL_FOUND)
    message (MESSAGE "FTGL not found! Disabled ftgl support!")
  endif ()

endif ()

if(FTGL_FOUND)
	include_directories (
	  ..
	  ${CMAKE_SOURCE_DIR}/OpenMesh/src
	  ${CMAKE_CURRENT_SOURCE_DIR}
	  ${CMAKE_CURRENT_SOURCE_DIR}/ShaderUtils
	  ${CMAKE_CURRENT_BINARY_DIR}
	  ${OPENGL_INCLUDE_DIR}
	  ${GLEW_INCLUDE_DIR}
	  ${GLUT_INCLUDE_DIR}
	  ${FREETYPE_INCLUDE_DIR_freetype2}
	  ${FREETYPE_INCLUDE_DIR_ft2build}
	)
else(FTGL_FOUND)
	include_directories (
	  ..
	  ${CMAKE_SOURCE_DIR}/OpenMesh/src
	  ${CMAKE_CURRENT_SOURCE_DIR}
	  ${CMAKE_CURRENT_SOURCE_DIR}/ShaderUtils
	  ${CMAKE_CURRENT_BINARY_DIR}
	  ${OPENGL_INCLUDE_DIR}
	  ${GLEW_INCLUDE_DIR}
	  ${GLUT_INCLUDE_DIR}
	)                                        
endif(FTGL_FOUND)

link_directories (
  ${GLEW_LIBRARY_DIR}
)

# source code directories
set (directories 
  .
  Geometry
  Config
  Geometry/Types
  GL
  Glut
  IO
  Math
  QtWidgets
  Scenegraph
  QtScenegraph
  ShaderUtils
  Utils
)

# generate dllexport macros on windows
if (WIN32)
  add_definitions(-DACGDLL)
endif ()

# collect all header,source and ui files
acg_append_files (headers "*.hh" ${directories})
acg_append_files (sources "*.cc" ${directories})
acg_append_files (ui "*.ui" ${directories})

# remove template cc files from source file list
acg_drop_templates (sources)

# genereate uic and moc targets
acg_qt4_autouic (uic_targets ${ui})
acg_qt4_automoc (moc_targets ${headers})

acg_add_library (ACG SHARED ${uic_targets} ${sources} ${headers} ${moc_targets})

if ( FTGL_FOUND )
	target_link_libraries (ACG 
	  OpenMeshCore
	  OpenMeshTools
	  ${QT_LIBRARIES}
	  ${OPENGL_LIBRARIES}
	  ${GLEW_LIBRARY}
	  ${GLUT_LIBRARIES}
	  ${FTGL_LIBS}
	  ${FREETYPE_LIBRARIES}
	)
else( FTGL_FOUND )
	target_link_libraries (ACG 
	  OpenMeshCore
	  OpenMeshTools
	  ${QT_LIBRARIES}
	  ${OPENGL_LIBRARIES}
	  ${GLEW_LIBRARY}
	  ${GLUT_LIBRARIES}
	)
endif( FTGL_FOUND )
