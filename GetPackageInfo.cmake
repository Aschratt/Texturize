###################################################################################################
#####                                                                                         #####
##### Call this function after a package has been found by CMake's FIND_PACKAGE in order to   #####
##### extract the include directories and binary names for the package.                       #####
#####                                                                                         #####
##### Syntax example:                                                                         #####
##### > SET(MODULE_INCLUDES)                                                                  #####
##### > SET(MODULE_BINARIES)                                                                  #####
##### > LIST(APPEND MODULE_NAMES LibFoo LibBar)                                               #####
##### > GET_PACKAGE_INFO("${MODULE_NAMES}" MODULE_INCLUDES MODULE_BINARIES)                   #####
#####                                                                                         #####
##### In the example above, the MODULE_INCLUDES will contain a list of include directories,   #####
##### one for each module provided in MODULE_NAMES. MODULE_BINARIES will contain the imported #####
##### library locations.                                                                      #####
#####                                                                                         #####
###################################################################################################

FUNCTION(GET_PACKAGE_INFO ARG_MODULE_NAMES RET_INCLUDES RET_INSTALLER)
  # Iterate each found module.
  FOREACH(_MODULE ${ARG_MODULE_NAMES})
    GET_PROPERTY(${_MODULE}_IS_IMPORTED TARGET ${_MODULE} PROPERTY IMPORTED)

    # Check, if the library is imported.
    IF(NOT ${_MODULE}_IS_IMPORTED)
      MESSAGE(WARNING "  WARNING: The module ${_MODULE} is expected to be imported, but seems to be part of the build. \
        It will be skipped, which may corrupt your build!")
    ELSE(NOT ${_MODULE}_IS_IMPORTED)
      # Get the include directories of the module.
      GET_PROPERTY(${_MODULE}_INCLUDE_DIRECTORIES TARGET ${_MODULE} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
      MESSAGE(STATUS "  [${_MODULE}] includes: ${${_MODULE}_INCLUDE_DIRECTORIES}")
      LIST(APPEND RET_INCLUDES ${${_MODULE}_INCLUDE_DIRECTORIES})
      
      # Resolve the assembly directories of the module.
      GET_PROPERTY(${_MODULE}_IMPORTED_LOCATION TARGET ${_MODULE} PROPERTY IMPORTED_LOCATION)

      # If there is no imported assembly, try again by importing for a current configuration.
      IF("${${_MODULE}_IMPORTED_LOCATION}" STREQUAL "")
        GET_PROPERTY(${_MODULE}_IMPORTED_LOCATION TARGET ${_MODULE} PROPERTY IMPORTED_LOCATION_${BUILD_TYPE})
      ENDIF("${${_MODULE}_IMPORTED_LOCATION}" STREQUAL "")

      # If an assembly has been found, add it to the install list.
      IF(NOT "${${_MODULE}_IMPORTED_LOCATION}" STREQUAL "")
        MESSAGE(STATUS "  [${_MODULE}] assemblies: ${${_MODULE}_IMPORTED_LOCATION}")
        LIST(APPEND RET_INSTALLER ${${_MODULE}_IMPORTED_LOCATION})
      ELSE(NOT "${${_MODULE}_IMPORTED_LOCATION}" STREQUAL "")
        MESSAGE(STATUS "  [${_MODULE}] assemblies: -")
      ENDIF(NOT "${${_MODULE}_IMPORTED_LOCATION}" STREQUAL "")
    ENDIF(NOT ${_MODULE}_IS_IMPORTED)
  ENDFOREACH(_MODULE ${MODULE_NAMES})
  
  # Propagate variables to parent scope.
  SET (${RET_INCLUDES} ${RET_INCLUDES} PARENT_SCOPE)
  SET (${RET_INSTALLER} ${RET_INSTALLER} PARENT_SCOPE)
ENDFUNCTION(GET_PACKAGE_INFO ARG_MODULE_NAMES RET_INCLUDES RET_INSTALLER)