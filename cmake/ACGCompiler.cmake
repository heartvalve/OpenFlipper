################################################################################
# Custom settings for compiler flags and similar
################################################################################

if (UNIX)

  IF( NOT CMAKE_SYSTEM MATCHES "SunOS*")

    # Pre initialize this variable   
    if ( NOT STL_VECTOR_CHECKS )
      set ( STL_VECTOR_CHECKS false CACHE BOOL "Include full stl vector checks in debug mode (This option is only used in debug Mode!)" )
    endif ( NOT STL_VECTOR_CHECKS )

    set (CMAKE_CFLAGS_RELEASE "-O3 -DINCLUDE_TEMPLATES -W -Wall -Wno-unused -DNDEBUG")
    set (CMAKE_CXX_FLAGS_RELEASE "-O3 -DINCLUDE_TEMPLATES -ftemplate-depth-100 -W -Wall -Wno-unused -DNDEBUG")

    if ( STL_VECTOR_CHECKS )
      set (CMAKE_C_FLAGS_DEBUG "-g -DINCLUDE_TEMPLATES -W -Wall -Wno-unused -DDEBUG -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC ")
      set (CMAKE_CXX_FLAGS_DEBUG "-g -DINCLUDE_TEMPLATES -ftemplate-depth-100 -W -Wall -Wno-unused -DDEBUG -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC")
    else ( STL_VECTOR_CHECKS )
      set (CMAKE_C_FLAGS_DEBUG "-g -DINCLUDE_TEMPLATES -W -Wall -Wno-unused -DDEBUG")
      set (CMAKE_CXX_FLAGS_DEBUG "-g -DINCLUDE_TEMPLATES -ftemplate-depth-100 -W -Wall -Wno-unused -DDEBUG")
    endif ( STL_VECTOR_CHECKS )
    
  ELSE ( NOT CMAKE_SYSTEM MATCHES "SunOS*")

    set (CMAKE_CFLAGS_RELEASE "-xO3 -DINCLUDE_TEMPLATES -DNDEBUG")
    set (CMAKE_CXX_FLAGS_RELEASE "-xO3 -DINCLUDE_TEMPLATES -DNDEBUG")
    set (CMAKE_C_FLAGS_DEBUG "-g -DINCLUDE_TEMPLATES -DDEBUG")
    set (CMAKE_CXX_FLAGS_DEBUG "-g -DINCLUDE_TEMPLATES -DDEBUG")
    
  endif ( NOT CMAKE_SYSTEM MATCHES "SunOS*" ) 

  if (APPLE)
    add_definitions( -DARCH_DARWIN )
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-non-virtual-dtor")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-non-virtual-dtor")
  endif ()
endif ()
