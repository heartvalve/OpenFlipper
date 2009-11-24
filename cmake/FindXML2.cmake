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

FIND_PATH( XML2_INCLUDE_DIR libxml/xmlreader.h
           PATHS "/usr/include/libxml2" )

 FIND_LIBRARY( XML2_LIBRARY
               NAMES xml2
               PATHS /usr/lib /usr/local/lib )


IF (XML2_INCLUDE_DIR AND XML2_LIBRARY)
  SET(XML2_FOUND TRUE)
ELSE (XML2_INCLUDE_DIR AND XML2_LIBRARY)
  SET( XML2_FOUND FALSE )
ENDIF (XML2_INCLUDE_DIR AND XML2_LIBRARY)

