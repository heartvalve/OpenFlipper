# - Try to find LIBUSB
# Once done this will define
#  LIBUSB_FOUND          - System has LIBUSB
#  LIBUSB_INCLUDE_DIRS   - The LIBUSB include directories
#  LIBUSB_COMPILER_FLAGS - Compiler flags that are necessary for LIBUSB

if (LIBUSB_INCLUDE_DIR)
  # in cache already
  set(LIBUSB_FOUND TRUE)
  set(LIBUSB_INCLUDE_DIRS "${LIBUSB_INCLUDE_DIR}" )
else (LIBUSB_INCLUDE_DIR)

find_path( LIBUSB_INCLUDE_DIR 
           NAMES usb.h
           PATHS $ENV{LIBUSB_DIR}
                 /usr/include
          )

find_library( LIBUSB_LIBRARY
              usb
              PATHS /usr/lib/ 
              )

          
set(LIBUSB_INCLUDE_DIRS "${LIBUSB_INCLUDE_DIR}" )
set(LIBUSB_LIBRARIES "${LIBUSB_LIBRARY}" )


include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set LIBUSB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBUSB  DEFAULT_MSG
                                  LIBUSB_INCLUDE_DIR LIBUSB_LIBRARY)

mark_as_advanced(LIBUSB_INCLUDE_DIR LIBUSB_LIBRARY)

endif(LIBUSB_INCLUDE_DIR)

