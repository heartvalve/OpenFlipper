# - Try to find XML2
# Once done this will define
#  
#  XML2_FOUND        - system has XML2
#  XML2_INCLUDE_DIR  - the XML2 include directory
#  XML2_LIBRARY      - Link these to use XML2
#   

IF (XML2_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(XML2_FIND_QUIETLY TRUE)
ENDIF (XML2_INCLUDE_DIRS)

 # Find the headers
 FIND_PATH( XML2_INCLUDE_DIR libxml/xmlversion.h
            PATHS /usr/include/libxml2 "C:/libs/libxml2/include"  )

if( WIN32 )
 FIND_LIBRARY( XML2_LIBRARY
               NAMES libxml2.lib
               PATHS "C:/libs/libxml2/lib" )  

 GET_FILENAME_COMPONENT( XML2_LIBRARY_DIR ${XML2_LIBRARY} PATH ) 
 list(APPEND XML2_LIBRARY_DIR "${XML2_LIBRARY_DIR}/../bin" )
else (WIN32)

 FIND_LIBRARY( XML2_LIBRARY
               NAMES xml2
               PATHS /usr/lib /usr/local/lib )
endif( WIN32)


IF (XML2_INCLUDE_DIR AND XML2_LIBRARY)
  SET(XML2_FOUND TRUE)
ELSE (XML2_INCLUDE_DIR AND XML2_LIBRARY)
  SET( XML2_FOUND FALSE )
ENDIF (XML2_INCLUDE_DIR AND XML2_LIBRARY)

