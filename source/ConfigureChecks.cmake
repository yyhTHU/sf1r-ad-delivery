##################################################
# Headers
#####
INCLUDE(CheckIncludeFile)

# int types
CHECK_INCLUDE_FILE(inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILE(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE(sys/stat.h HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE(stddef.h HAVE_STDDEF_H)

# signal.h
CHECK_INCLUDE_FILE(signal.h HAVE_SIGNAL_H)

# ext hash
CHECK_INCLUDE_FILE(ext/hash_map HAVE_EXT_HASH_MAP)

##################################################
# Our Proprietary Libraries
#####

FIND_PACKAGE(izenelib REQUIRED COMPONENTS
  index_manager
  mock_index_manager
  febird
  udt3
  izene_log
  bigint
  procmeminfo
  izene_util
  ticpp
  luxio
  jemalloc
  json
  ticpp
  am
  lzo
  boost_memory
  )

FIND_PACKAGE(ilplib REQUIRED COMPONENTS
  ilplib
)

FIND_PACKAGE(idmlib REQUIRED)
FIND_PACKAGE(imllib REQUIRED)
FIND_PACKAGE(xml2 REQUIRED)

IF( USE_IISE )
  FIND_PACKAGE(iise REQUIRED)
  ADD_DEFINITIONS( -DUSE_IISELIB=TRUE )
ELSE( USE_IISE )
  SET( iise_LIBRARIES "" )
  SET( iise_INCLUDE_DIRS "" )
  SET( iise_LIBRARY_DIRS "" )
ENDIF( USE_IISE )

IF( USE_WISEKMA )
  FIND_PACKAGE(wisekma REQUIRED)
  ADD_DEFINITIONS( -DUSE_WISEKMA=TRUE )
ELSE( USE_WISEKMA )
  SET( wisekma_LIBRARIES "" )
  SET( wisekma_INCLUDE_DIRS "" )
  SET( wisekma_LIBRARY_DIRS "" )
ENDIF( USE_WISEKMA )

IF( USE_IZENECMA )
  FIND_PACKAGE(izenecma REQUIRED)
  ADD_DEFINITIONS( -DUSE_IZENECMA=TRUE )
ELSE( USE_IZENECMA )
  SET( izenecma_INCLUDE_DIRS "" )
  SET( izenecma_LIBRARIES "" )
  SET( izenecma_LIBRARY_DIRS "" )
ENDIF( USE_IZENECMA )

##################################################
# Other Libraries
#####

FIND_PACKAGE(Threads REQUIRED)

SET(Boost_ADDITIONAL_VERSIONS 1.40 1.40.0 1.39 1.39.0 1.38 1.38.0 1.37 1.37.0)
FIND_PACKAGE(Boost 1.38 REQUIRED
  COMPONENTS
  system
  program_options
  thread
  regex
  date_time
  serialization
  filesystem
  unit_test_framework
  iostreams
  )

FIND_PACKAGE(TokyoCabinet 1.4.29 REQUIRED)
FIND_PACKAGE(Glog REQUIRED)
FIND_PACKAGE(sqlite3 REQUIRED)
FIND_PACKAGE(LibCURL REQUIRED)

##################################################
# Driver Docs
#####
GET_FILENAME_COMPONENT(SF1V5_PARENT_DIR "${SF1V5_ROOT}" PATH)
SET(SF1V5_DRIVER_DOCS_ROOT "${SF1V5_PARENT_DIR}/sf1-driver-docs/")

##################################################
# Doxygen
#####
FIND_PACKAGE(Doxygen)
IF(DOXYGEN_DOT_EXECUTABLE)
  OPTION(USE_DOT "use dot in doxygen?" FLASE)
ENDIF(DOXYGEN_DOT_EXECUTABLE)

SET(USE_DOT_YESNO NO)
IF(USE_DOT)
  SET(USE_DOT_YESNO YES)
ENDIF(USE_DOT)