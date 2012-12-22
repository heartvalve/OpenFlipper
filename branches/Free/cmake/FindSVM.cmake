# - Try to find SVM
# Once done this will define
#  
#  SVM_FOUND        - system has SVM
#  SVM_INCLUDE_DIR  - the SVM include directory
#  SVM_LIBRARY      - Link these to use SVM
#   

IF (SVM_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(SVM_FIND_QUIETLY TRUE)
ENDIF (SVM_INCLUDE_DIRS)

FIND_PATH( SVM_INCLUDE_DIR libsvm/svm.h
           PATHS /usr/include/libsvm-2.0
                 /usr/include/libsvm-3.0
                 /usr/include
                 /opt/local/include )


FIND_LIBRARY( SVM_LIBRARY
              NAMES svm
              PATHS /lib /usr/lib /usr/local/lib /opt/local/lib )


IF (SVM_INCLUDE_DIR AND SVM_LIBRARY)
  SET(SVM_FOUND TRUE)
ELSE (SVM_INCLUDE_DIR AND SVM_LIBRARY)
  SET( SVM_FOUND FALSE )
ENDIF (SVM_INCLUDE_DIR AND SVM_LIBRARY)

