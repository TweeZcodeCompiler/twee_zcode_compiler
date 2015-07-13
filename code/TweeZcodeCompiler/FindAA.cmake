find_path(AA_INCLUDE_DIR aalib.h
	/usr/include
	/usr/local/include)

find_library(AA_LIBRARY aa PATHS
    /usr/lib/
    /usr/local/lib)

if(AA_INCLUDE_DIR AND AA_LIBRARY)
	set(AA_FOUND TRUE)
endif()


if(AA_FOUND)
   if(NOT AA_FIND_QUIETLY)
      message(STATUS "Found AA: ${AA_LIBRARY}")
   endif()
else(AA_FOUND)
   if(AA_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find AA")
   endif()
endif()