###################################################################################################
#####                                                                                         #####
##### The script sets up the frameworks project hierarchy. Currently only Windows builds have #####
##### been tested. To layout the projects, consider using the `cmake.ps1` script.             #####
#####                                                                                         #####
###################################################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

# Lookup project version.
INCLUDE("cmake/TexturizeVersion.cmake")

# Configure the project
PROJECT(Texturize LANGUAGES CXX VERSION ${Texturize_VERSION})

MESSAGE(STATUS "---------------------------------------------------------------------------------------------------")
MESSAGE(STATUS "Building Texturize Version ${Texturize_VERSION_MAJOR}.${Texturize_VERSION_MINOR}")
MESSAGE(STATUS "---------------------------------------------------------------------------------------------------")
MESSAGE(STATUS "")

# Defaulting build type to release.
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release")
  MESSAGE(STATUS "  Build type not specified: Defaulting to `Release`.")
  MESSAGE(STATUS "")
ENDIF(NOT CMAKE_BUILD_TYPE)

# Get the config string from the build profile.
STRING(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

# Set compiler flags
IF(MSVC)
  IF(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.00)
    INCLUDE(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    
    IF(_cpp_latest_flag_supported)
      ADD_COMPILE_OPTIONS("/std:c++latest")
    ENDIF(_cpp_latest_flag_supported)
  ELSEIF(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.00)
    MESSAGE(FATAL_ERROR "Microsoft Visual Studio version \"1900\" or newer is required, but only \"${CMAKE_CXX_COMPILER_VERSION}\" was detected.")
  ENDIF(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.00)

  # For Visual Studio 19.6 or newer, use "/external:I " before 3rd party includes in order to suppress warnings.
  IF(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.14)
    # For now, only do this in release modes.
    IF(NOT ${BUILD_TYPE} EQUAL "DEBUG")
      # TODO: This probably changes with newer versions...
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /experimental:external /external:W0 ")
      SET(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "/external:I ")
    ENDIF(NOT ${BUILD_TYPE} EQUAL "DEBUG")
  ELSE(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.14)
    SET(CMAKE_INCLUDE_SYSTEM_FLAG_CXX " ")
  ENDIF(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.14)

  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W3 /Od /EHsc")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W3 /GL /Oi /Gy /EHsc")
  SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /OPT:NOREF")
  SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /OPT:NOREF")
  SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG /INCREMENTAL:NO /OPT:REF")
  SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /INCREMENTAL:NO /OPT:REF")

  # Eigen produces can produce a build error C1128 in debug builds.
  IF(WITH_TAPKEE)
	  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
  ENDIF(WITH_TAPKEE)
ELSE(MSVC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

  IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
ENDIF(MSVC)

# Ensure that the build is out of source.
STRING(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" IS_IN_SOURCE_BUILD)

IF(NOT IS_IN_SOURCE_BUILD)
  GET_FILENAME_COMPONENT(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)
  STRING(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PARENT_DIR}" IS_IN_SOURCE_BUILD)
ENDIF(NOT IS_IN_SOURCE_BUILD)

IF(IS_IN_SOURCE_BUILD)
  MESSAGE(FATAL_ERROR "FATAL: In source builds are not allowed.")
ENDIF(IS_IN_SOURCE_BUILD)

# Builds appear to be good, setup environment variables.
# If the install prefix is not set, set it relative to the binary directory.
# If it is set, but relative, append it to the binary directory.
# If it is set to an absolute path, don't change anything.
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install directory")
ELSEIF(NOT IS_ABSOLUTE ${CMAKE_INSTALL_PREFIX})
  SET(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)

# Setup output directories.
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY      "${CMAKE_BINARY_DIR}/build/lib/")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY      "${CMAKE_BINARY_DIR}/build/bin/")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY      "${CMAKE_BINARY_DIR}/build/bin/")
SET(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY   "${CMAKE_BINARY_DIR}/build/bin/")
SET(CMAKE_PDB_OUTPUT_DIRECTORY          "${CMAKE_BINARY_DIR}/build/bin/")

LIST(APPEND CMAKE_MODULE_PATH           "${CMAKE_SOURCE_DIR}/modules")

MESSAGE(STATUS "  Source directory:     ${CMAKE_SOURCE_DIR}")
MESSAGE(STATUS "  Build directory:      ${CMAKE_BINARY_DIR}")
MESSAGE(STATUS "  Install directory:    ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "")

# Include macros and functions
MESSAGE(STATUS "  Looking up macros and functions...")
MESSAGE(STATUS "  > ${CMAKE_SOURCE_DIR}/GetModuleInfo.cmake")
INCLUDE("${CMAKE_SOURCE_DIR}/cmake/GetPackageInfo.cmake")
MESSAGE(STATUS "  > ${CMAKE_SOURCE_DIR}/DefineApp.cmake")
INCLUDE("${CMAKE_SOURCE_DIR}/cmake/DefineApp.cmake")
MESSAGE(STATUS "  > ${CMAKE_SOURCE_DIR}/TexturizeSettings.cmake")
INCLUDE("${CMAKE_SOURCE_DIR}/cmake/TexturizeSettings.cmake")
MESSAGE(STATUS "")

# Searching for modules.
MESSAGE(STATUS "---------------------------------------------------------------------------------------------------")
MESSAGE(STATUS "Looking for modules")
MESSAGE(STATUS "---------------------------------------------------------------------------------------------------")
MESSAGE(STATUS "")
MESSAGE(STATUS "  Modules directory:    ${CMAKE_MODULE_PATH}")
MESSAGE(STATUS "")
MESSAGE(STATUS "")
MESSAGE(STATUS "  OpenCV:               ${OpenCV_DIR}")
MESSAGE(STATUS "  OpenEXR:              ${OPENEXR_LOCATION}")
MESSAGE(STATUS "  TBB:                  ${TBB_DIR}")
MESSAGE(STATUS "  HDF5:                 ${HDF5_ROOT}")
MESSAGE(STATUS "  zlib:                 ${ZLIB_ROOT}")
MESSAGE(STATUS "  tapkee:               ${TAPKEE_DIR}")
MESSAGE(STATUS "")
MESSAGE(STATUS "---------------------------------------------------------------------------------------------------")
MESSAGE(STATUS "Configuring projects...")

###################################################################################################
##### Setup project hierarchy.                                                                #####
###################################################################################################

# Framework libraries
ADD_SUBDIRECTORY(libs/Texturize.Core)
ADD_SUBDIRECTORY(libs/Texturize.Codecs)
ADD_SUBDIRECTORY(libs/Texturize.Analysis)
ADD_SUBDIRECTORY(libs/Texturize.Sampling)
ADD_SUBDIRECTORY(libs/Texturize.Codecs.EXR)

#SET_TARGET_PROPERTIES(Texturize.Core Texturize.Codecs Texturize.Analysis Texturize.Sampling Texturize.Codecs.EXR PROPERTIES FOLDER "libs")

DEFINE_APP(BUILD_ZLIB                            ${ZLIB_ROOT})

DEFINE_APP(WITH_TAPKEE                           "adapters/Texturize.Adapters.Tapkee")

DEFINE_APP(BUILD_APP_SANDBOX                     "apps/Texturize.Sandbox")
DEFINE_APP(BUILD_APP_FILTERMR8                   "apps/Texturize.FilterMR8")
DEFINE_APP(BUILD_APP_KMEANS                      "apps/Texturize.KMeans")
DEFINE_APP(BUILD_APP_DISTANCE_MATRIX             "apps/Texturize.Distance")
DEFINE_APP(BUILD_APP_PROGRESSION_MAP             "apps/Texturize.ProgressionMap")
DEFINE_APP(BUILD_APP_GUIDANCE_REFINE             "apps/Texturize.GuidanceRefine")
DEFINE_APP(BUILD_APP_APPEARANCE_SPACE            "apps/Texturize.AppearanceSpace")
DEFINE_APP(BUILD_APP_SYNTHESIZE                  "apps/Texturize.Synthesize")
DEFINE_APP(BUILD_APP_UV_MAP                      "apps/Texturize.UVMap")

#SET_TARGET_PROPERTIES(Texturize.Adapters.Tapkee PROPERTIES FOLDER "adapter")
#SET_TARGET_PROPERTIES(Texturize.Sandbox PROPERTIES FOLDER "legacy")
#SET_TARGET_PROPERTIES(Texturize.FilterMR8 Texturize.KMeans Texturize.Distance Texturize.ProgressionMap Texturize.GuidanceRefine PROPERTIES FOLDER "apps")