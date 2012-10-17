# Find the Qwt 5.x includes and library, either the version linked to Qt3 or the version linked to Qt4
#
# On Windows it makes these assumptions:
#    - the Qwt DLL is where the other DLLs for Qt are (QT_DIR\bin) or in the path
#    - the Qwt .h files are in QT_DIR\include\Qwt or in the path
#    - the Qwt .lib is where the other LIBs for Qt are (QT_DIR\lib) or in the path
#
# Qwt5_INCLUDE_DIR - where to find qwt.h if Qwt
# Qwt5_Qt4_LIBRARY - The Qwt5 library linked against Qt4 (if it exists)#
# Qwt5_Qt4_LIBRARY_DIR 
# Qwt5_Qt3_LIBRARY - The Qwt5 library linked against Qt4 (if it exists)
# Qwt5_Qt4_FOUND   - Qwt5 was found and uses Qt4
# Qwt5_Qt3_FOUND   - Qwt5 was found and uses Qt3
# Qwt5_FOUND - Set to TRUE if Qwt5 was found (linked either to Qt3 or Qt4)

# Copyright (c) 2007, Pau Garcia i Quiles, <pgquiles@elpauer.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# Condition is "(A OR B) AND C", CMake does not support parentheses but it evaluates left to right
IF(Qwt5_Qt4_LIBRARY OR Qwt5_Qt3_LIBRARY AND Qwt5_INCLUDE_DIR)
    SET(Qwt5_FIND_QUIETLY TRUE)
ENDIF(Qwt5_Qt4_LIBRARY OR Qwt5_Qt3_LIBRARY AND Qwt5_INCLUDE_DIR)

IF(NOT QT4_FOUND)
	FIND_PACKAGE( Qt4 REQUIRED QUIET )
ENDIF(NOT QT4_FOUND)

IF( QT4_FOUND )
    if ( CMAKE_GENERATOR MATCHES "Visual Studio 9*" )
	  set( SEARCHPATH "./" )
	endif()
	  
	if ( CMAKE_GENERATOR MATCHES "Visual Studio 10*" )
	  set( SEARCHPATH "VS2010" )
	endif()

	# Is Qwt5 installed? Look for header files
	FIND_PATH( Qwt5_INCLUDE_DIR qwt.h 
               PATHS ${QT_INCLUDE_DIR} 
				/usr/local/qwt/include 
				/usr/local/include 
                                /opt/local/include
                                /opt/local/include/qwt
				/usr/include
				/usr/include 
                                c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.0.2
				c:\\libs\\Qwt-6.0.1
				c:\\Program\ Files\\qwt\\qwt-5.2.0 
				c:\\qwt-5.2.0 
				C:\\libs\\qwt-5.2.0
               PATH_SUFFIXES qwt qwt5 qwt-qt4 qwt5-qt4 qwt-qt3 qwt5-qt3 include qwt/include qwt5/include qwt-qt4/include qwt5-qt4/include qwt-qt3/include qwt5-qt3/include ENV PATH)

	# Find Qwt version
	IF( Qwt5_INCLUDE_DIR )
		FILE( READ ${Qwt5_INCLUDE_DIR}/qwt_global.h QWT_GLOBAL_H )
		STRING( REGEX MATCH "#define *QWT_VERSION *(0x05*)" QWT_IS_VERSION_5 ${QWT_GLOBAL_H})
		
		IF( QWT_IS_VERSION_5 )
		STRING(REGEX REPLACE ".*#define[\\t\\ ]+QWT_VERSION_STR[\\t\\ ]+\"([0-9]+\\.[0-9]+\\.[0-9]+)\".*" "\\1" Qwt_VERSION "${QWT_GLOBAL_H}")

		IF( APPLE ) #Apple
		    # Find Qwt5 library linked to Qt4
		    FIND_LIBRARY( Qwt5_Qt4_TENTATIVE_LIBRARY_RELEASE NAMES qwt5-qt4 qwt-qt4 qwt5 qwt PATHS /usr/local/qwt/lib /opt/local/lib /usr/local/lib /usr/lib )

                    FIND_LIBRARY( Qwt5_Qt4_TENTATIVE_LIBRARY_DEBUG NAMES qwt_debug PATHS /usr/local/qwt/lib /opt/local/lib /usr/local/lib /usr/lib )

                    get_filename_component(_Qwt5_Qt4_LIBRARY_DIR ${Qwt5_Qt4_TENTATIVE_LIBRARY_RELEASE} PATH)
                    set ( Qwt5_Qt4_LIBRARY_DIR ${_Qwt5_Qt4_LIBRARY_DIR} CACHE FILEPATH "Library dir of qwt" )
 
                    if ( Qwt5_Qt4_TENTATIVE_LIBRARY_RELEASE AND Qwt5_Qt4_TENTATIVE_LIBRARY_DEBUG )
                      set ( Qwt5_Qt4_TENTATIVE_LIBRARY "optimized;${Qwt5_Qt4_TENTATIVE_LIBRARY_RELEASE};debug;${Qwt5_Qt4_TENTATIVE_LIBRARY_DEBUG}")
                    else()
                      set ( Qwt5_Qt4_TENTATIVE_LIBRARY "${Qwt5_Qt4_TENTATIVE_LIBRARY_RELEASE}")
                    endif()
 
                ELSEIF( WIN32 )
		    # Find Qwt5 library linked to Qt4 Release Version
                    FIND_LIBRARY( Qwt5_Qt4_TENTATIVE_LIBRARY_RELEASE NAMES qwt qwt5-qt4 qwt-qt4 qwt5  PATHS 
                               "c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.0.2\\lib"
							   "c:\\libs\\Qwt-6.0.1\\lib"
				               "c:\\Program\ Files\\qwt\\qwt-5.2.0\\lib" 
							   "c:\\libs\\qwt-5.2.0\\lib"
							   )
		    # Find Qwt5 library linked to Qt4 Debug Version
		    FIND_LIBRARY( Qwt5_Qt4_TENTATIVE_LIBRARY_DEBUG NAMES qwtd qwt5d  PATHS 
                            "c:\\libs\\${SEARCHPATH}\\x32\\Qwt-6.0.2\\lib"
			                "c:\\libs\\Qwt-6.0.1\\lib"
			                "c:\\Program\ Files\\qwt\\qwt-5.2.0\\lib" 
							"c:\\libs\\qwt-5.2.0\\lib")

                    set ( Qwt5_Qt4_TENTATIVE_LIBRARY "optimized;${Qwt5_Qt4_TENTATIVE_LIBRARY_RELEASE};debug;${Qwt5_Qt4_TENTATIVE_LIBRARY_DEBUG}")
                    get_filename_component(_Qwt5_Qt4_LIBRARY_DIR ${Qwt5_Qt4_TENTATIVE_LIBRARY_RELEASE} PATH)
                    set ( Qwt5_Qt4_LIBRARY_DIR ${_Qwt5_Qt4_LIBRARY_DIR} CACHE FILEPATH "Library dir of qwt" )
                ELSE() #Linux
                    # Find Qwt5 library linked to Qt4
                    FIND_LIBRARY( Qwt5_Qt4_TENTATIVE_LIBRARY NAMES qwt5-qt4 qwt-qt4 qwt5 qwt PATHS /usr/local/qwt/lib /opt/local/lib /usr/local/lib /usr/lib )
                    get_filename_component(_Qwt5_Qt4_LIBRARY_DIR ${Qwt5_Qt4_TENTATIVE_LIBRARY} PATH)
                    set ( Qwt5_Qt4_LIBRARY_DIR ${_Qwt5_Qt4_LIBRARY_DIR} CACHE FILEPATH "Library dir of qwt" )
		ENDIF()

		IF( UNIX AND NOT CYGWIN AND NOT APPLE)
			IF( Qwt5_Qt4_TENTATIVE_LIBRARY )
				#MESSAGE("Qwt5_Qt4_TENTATIVE_LIBRARY = ${Qwt5_Qt4_TENTATIVE_LIBRARY}")
				EXECUTE_PROCESS( COMMAND "ldd" ${Qwt5_Qt4_TENTATIVE_LIBRARY} OUTPUT_VARIABLE Qwt_Qt4_LIBRARIES_LINKED_TO )
				STRING( REGEX MATCH "QtCore" Qwt5_IS_LINKED_TO_Qt4 ${Qwt_Qt4_LIBRARIES_LINKED_TO})
				IF( Qwt5_IS_LINKED_TO_Qt4 )
					SET( Qwt5_Qt4_LIBRARY ${Qwt5_Qt4_TENTATIVE_LIBRARY} CACHE FILEPATH "Qt4 linked version of the qwt lib")
					SET( Qwt5_Qt4_FOUND TRUE )
					IF (NOT Qwt5_FIND_QUIETLY)
						MESSAGE( STATUS "Found Qwt: ${Qwt5_Qt4_LIBRARY}" )
					ENDIF (NOT Qwt5_FIND_QUIETLY)
				ENDIF( Qwt5_IS_LINKED_TO_Qt4 )
			ENDIF( Qwt5_Qt4_TENTATIVE_LIBRARY )
		ELSE( UNIX AND NOT CYGWIN AND NOT APPLE )
                        IF ( WIN32 )
                                IF( Qwt5_Qt4_TENTATIVE_LIBRARY )
 		        		# Assumes qwt.dll is in the Qt dir
					SET( Qwt5_Qt4_LIBRARY ${Qwt5_Qt4_TENTATIVE_LIBRARY} CACHE FILEPATH "Qt4 linked version of the qwt lib")
			  		SET( Qwt5_Qt4_FOUND TRUE )
			  		IF (NOT Qwt5_FIND_QUIETLY)
						MESSAGE( STATUS "Found Qwt version ${Qwt_VERSION} linked to Qt4" )
			  	 	ENDIF (NOT Qwt5_FIND_QUIETLY)
                                ENDIF( Qwt5_Qt4_TENTATIVE_LIBRARY )
                        ENDIF( WIN32 )

                        IF ( APPLE )
                                IF( Qwt5_Qt4_TENTATIVE_LIBRARY )
					SET( Qwt5_Qt4_LIBRARY ${Qwt5_Qt4_TENTATIVE_LIBRARY} CACHE FILEPATH "Qt4 linked version of the qwt lib")
			  		SET( Qwt5_Qt4_FOUND TRUE )
			  		IF (NOT Qwt5_FIND_QUIETLY)
						MESSAGE( STATUS "Found Qwt version ${Qwt_VERSION} linked to Qt4" )
			  		ENDIF (NOT Qwt5_FIND_QUIETLY)
                                ENDIF( Qwt5_Qt4_TENTATIVE_LIBRARY )
                        ENDIF( APPLE )

		ENDIF( UNIX AND NOT CYGWIN AND NOT APPLE)

		
                IF ( NOT APPLE )
			# Find Qwt5 library linked to Qt3
			FIND_LIBRARY( Qwt5_Qt3_TENTATIVE_LIBRARY NAMES qwt-qt3 qwt qwt5-qt3 qwt5 qwt )
			IF( UNIX AND NOT CYGWIN)
				IF( Qwt5_Qt3_TENTATIVE_LIBRARY )
					#MESSAGE("Qwt5_Qt3_TENTATIVE_LIBRARY = ${Qwt5_Qt3_TENTATIVE_LIBRARY}")
					EXECUTE_PROCESS( COMMAND "ldd" ${Qwt5_Qt3_TENTATIVE_LIBRARY} OUTPUT_VARIABLE Qwt-Qt3_LIBRARIES_LINKED_TO )
					STRING( REGEX MATCH "qt-mt" Qwt5_IS_LINKED_TO_Qt3 ${Qwt-Qt3_LIBRARIES_LINKED_TO})
					IF( Qwt5_IS_LINKED_TO_Qt3 )
						SET( Qwt5_Qt3_LIBRARY ${Qwt5_Qt3_TENTATIVE_LIBRARY} CACHE FILEPATH "Qt3 linked version of the qwt lib")
						SET( Qwt5_Qt3_FOUND TRUE )
						IF (NOT Qwt5_FIND_QUIETLY)
							MESSAGE( STATUS "Found Qwt version ${Qwt_VERSION} linked to Qt3" )
						ENDIF (NOT Qwt5_FIND_QUIETLY)
					ENDIF( Qwt5_IS_LINKED_TO_Qt3 )
				ENDIF( Qwt5_Qt3_TENTATIVE_LIBRARY )
			ELSE( UNIX AND NOT CYGWIN)
				SET( Qwt5_Qt3_LIBRARY ${Qwt5_Qt3_TENTATIVE_LIBRARY} CACHE FILEPATH "Qt3 linked version of the qwt lib")
				SET( Qwt5_Qt3_FOUND TRUE )
				IF (NOT Qwt5_FIND_QUIETLY)
					MESSAGE( STATUS "Found Qwt: ${Qwt5_Qt3_LIBRARY}" )
				ENDIF (NOT Qwt5_FIND_QUIETLY)
			ENDIF( UNIX AND NOT CYGWIN )
		ENDIF( NOT APPLE )
		
		ENDIF( QWT_IS_VERSION_5 )
		
		IF( Qwt5_Qt4_FOUND OR Qwt5_Qt3_FOUND )
			SET( Qwt5_FOUND TRUE )
		ENDIF( Qwt5_Qt4_FOUND OR Qwt5_Qt3_FOUND )
		
		MARK_AS_ADVANCED( Qwt5_INCLUDE_DIR Qwt5_Qt4_LIBRARY Qwt5_Qt3_LIBRARY )
	ENDIF( Qwt5_INCLUDE_DIR )

   	IF (NOT Qwt5_FOUND AND Qwt5_FIND_REQUIRED)
      		MESSAGE(FATAL_ERROR "Could not find Qwt 5.x")
   	ENDIF (NOT Qwt5_FOUND AND Qwt5_FIND_REQUIRED)

ENDIF( QT4_FOUND )
