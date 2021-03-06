# - Find the Poco includes and libraries.
# The following variables are set if Poco is found.  If Poco is not
# found, Poco_FOUND is set to false.
#  Poco_FOUND        - True when the Poco include directory is found.
#  Poco_INCLUDE_DIRS - the path to where the poco include files are.
#  Poco_LIBRARY_DIRS - The path to where the poco library files are.
#  Poco_BINARY_DIRS - The path to where the poco dlls are.
#  Poco_LIBRARIES - All libraries

# ----------------------------------------------------------------------------

# Usage:
# In your CMakeLists.txt file do something like this:
# ...
# # Poco
# FIND_PACKAGE(Poco)
# ...
# INCLUDE_DIRECTORIES(${Poco_INCLUDE_DIRS})
# LINK_DIRECTORIES(${Poco_LIBRARY_DIRS})
#
# In Windows, we make the assumption that, if the Poco files are installed, the default directory
# will be C:\poco or C:\Program Files\Poco.

SET(POCO_INCLUDE_PATH_DESCRIPTION "top-level directory containing the poco include directories. E.g /usr/local/include/poco-1.2.1 or c:\\poco\\include\\poco-1.2.1")
SET(POCO_INCLUDE_DIR_MESSAGE "Set the Poco_INCLUDE_DIR cmake cache entry to the ${POCO_INCLUDE_PATH_DESCRIPTION}")
SET(POCO_LIBRARY_PATH_DESCRIPTION "top-level directory containing the poco libraries.")
SET(POCO_LIBRARY_DIR_MESSAGE "Set the Poco_LIBRARY_DIR cmake cache entry to the ${POCO_LIBRARY_PATH_DESCRIPTION}")


SET(POCO_DIR_SEARCH $ENV{POCO_ROOT})
IF(POCO_DIR_SEARCH)
  FILE(TO_CMAKE_PATH ${POCO_DIR_SEARCH} POCO_DIR_SEARCH)
ELSE(POCO_DIR_SEARCH)
  SET(POCO_DIR_SEARCH ${POCO_ROOT})
  IF(POCO_DIR_SEARCH)
    FILE(TO_CMAKE_PATH ${POCO_DIR_SEARCH} POCO_DIR_SEARCH)
  ENDIF(POCO_DIR_SEARCH)
ENDIF(POCO_DIR_SEARCH)


IF(WIN32)
  SET(POCO_DIR_SEARCH
    ${POCO_DIR_SEARCH}
    C:/poco
    D:/poco
    "C:Program Files/poco"
    "D:Program Files/poco"
  )
ENDIF(WIN32)

# Add in some path suffixes. These will have to be updated whenever a new Poco version comes out.
SET(SUFFIX_FOR_INCLUDE_PATH
 poco-1.2.4
 poco-1.2.3
 poco-1.2.1
)

SET(SUFFIX_FOR_LIBRARY_PATH
 poco-1.2.4/lib
 poco-1.2.4/lib/Linux/i686
 poco-1.2.3/lib
 poco-1.2.3/lib/Linux/i686
 poco-1.2.1/lib
 poco-1.2.1/lib/Linux/i686
 lib
 lib/Linux/i686
)

#
# Look for an installation.
#
FIND_PATH(Poco_INCLUDE_DIR NAMES Poco/AbstractCache.h Foundation/include/Poco/AbstractCache.h PATH_SUFFIXES ${SUFFIX_FOR_INCLUDE_PATH} PATHS

  # Look in other places.
  ${POCO_DIR_SEARCH}

  # Help the user find it if we cannot.
  DOC "The ${POCO_INCLUDE_DIR_MESSAGE}"
)

# Assume we didn't find it.
SET(Poco_FOUND 0)

# Now try to get the include and library path.
IF(Poco_INCLUDE_DIR)

  IF(EXISTS "${Poco_INCLUDE_DIR}")
    SET(Poco_INCLUDE_DIRS
      ${Poco_INCLUDE_DIR}/CppUnit/include
      ${Poco_INCLUDE_DIR}/Foundation/include
      ${Poco_INCLUDE_DIR}/Net/include
      ${Poco_INCLUDE_DIR}/Util/include
      ${Poco_INCLUDE_DIR}/XML/include
      ${Poco_INCLUDE_DIR}/Zip/include
    )
    SET(Poco_FOUND 1)
  ENDIF(EXISTS "${Poco_INCLUDE_DIR}")

  set( Poco_LIBRARIES "") #reset
  IF ( NOT Poco_LIBRARIES )
#   FIND_LIBRARY(Poco_LIBRARY_Library NAMES PocoLibrary PocoLibraryd PATH_SUFFIXES ${SUFFIX_FOR_LIBRARY_PATH} PATHS
#       # Look in other places.
#       ${Poco_INCLUDE_DIR}
#       ${POCO_DIR_SEARCH}
#       # Help the user find it if we cannot.
#       DOC "The ${POCO_LIBRARY_PATH_DESCRIPTION}"
#     )
# set( Poco_LIBRARIES ...;${Poco_LIBRARY_Library}
#     FIND_LIBRARY(Poco_LIBRARY_Zip NAMES PocoZip PocoZipd PATH_SUFFIXES ${SUFFIX_FOR_LIBRARY_PATH} PATHS
#       # Look in other places.
#       ${Poco_INCLUDE_DIR}
#       ${POCO_DIR_SEARCH}
#       # Help the user find it if we cannot.
#       DOC "The ${POCO_LIBRARY_PATH_DESCRIPTION}"
#     )
# set( Poco_LIBRARIES ...;${Poco_LIBRARY_Zip}
#   FIND_LIBRARY(Poco_LIBRARY_Util NAMES PocoUtil PocoUtild PATH_SUFFIXES ${SUFFIX_FOR_LIBRARY_PATH} PATHS
#       # Look in other places.
#       ${Poco_INCLUDE_DIR}
#       ${POCO_DIR_SEARCH}
#       # Help the user find it if we cannot.
#       DOC "The ${POCO_LIBRARY_PATH_DESCRIPTION}"
#     )
# set( Poco_LIBRARIES ...;${Poco_LIBRARY_Util}
  FIND_LIBRARY(Poco_LIBRARY_Found NAMES PocoFoundation PocoFoundationd PATH_SUFFIXES ${SUFFIX_FOR_LIBRARY_PATH} PATHS
      # Look in other places.
      ${Poco_INCLUDE_DIR}
      ${POCO_DIR_SEARCH}
      # Help the user find it if we cannot.
      DOC "The ${POCO_LIBRARY_PATH_DESCRIPTION}"
    )
  set( Poco_LIBRARIES     "${Poco_LIBRARY_Found}" CACHE FILEPATH "Poco libraries names" FORCE )

  endif()

  IF ( NOT Poco_LIBRARY_DIR )
    FIND_LIBRARY(Poco_LIBRARY_DIR_TMP NAMES PocoFoundation PocoFoundationd PocoZip PocoZipd PATH_SUFFIXES ${SUFFIX_FOR_LIBRARY_PATH} PATHS

      # Look in other places.
      ${Poco_INCLUDE_DIR}
      ${POCO_DIR_SEARCH}

      # Help the user find it if we cannot.
      DOC "The ${POCO_LIBRARY_PATH_DESCRIPTION}"
    )
    GET_FILENAME_COMPONENT(Poco_LIBRARY_DIR_PATH ${Poco_LIBRARY_DIR_TMP} PATH)
    SET( Poco_LIBRARY_DIR ${Poco_LIBRARY_DIR_PATH} CACHE PATH "Poco library dir path" )
  endif()
  
  IF(Poco_INCLUDE_DIR)
    
	
    # Look for the poco binary path.
    SET(Poco_BINARY_DIR ${Poco_INCLUDE_DIR})
    IF(Poco_BINARY_DIR AND EXISTS "${Poco_BINARY_DIR}")

      SET(Poco_BINARY_DIRS ${Poco_BINARY_DIR}/bin)
	  SET(Poco_LIBRARY_DIRS "${Poco_BINARY_DIR}/bin" CACHE PATH "All Poco Library Dirs")
	  
    ENDIF(Poco_BINARY_DIR AND EXISTS "${Poco_BINARY_DIR}")

  ENDIF(Poco_INCLUDE_DIR)

ENDIF(Poco_INCLUDE_DIR)

IF(NOT Poco_FOUND)
  IF(NOT Poco_FIND_QUIETLY)
    MESSAGE(STATUS "Poco was not found. ${POCO_DIR_MESSAGE}")
  ELSE(NOT Poco_FIND_QUIETLY)
    IF(Poco_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Poco was not found. ${POCO_DIR_MESSAGE}")
    ENDIF(Poco_FIND_REQUIRED)
  ENDIF(NOT Poco_FIND_QUIETLY)
ENDIF(NOT Poco_FOUND)

IF(NOT Poco_FIND_QUIETLY)
  message( ${Poco_INCLUDE_DIR} + " and lib dir " + ${Poco_LIBRARY_DIR} + " poco found " + ${Poco_FOUND})
ENDIF(NOT Poco_FIND_QUIETLY)
