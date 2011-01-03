# vars
SET ( EXAMPLE_NAME			CastlesClient )
SET ( EXAMPLE_INSTALL_DIR	${EXAMPLE_INSTALL_DIR}/Castles/${EXAMPLE} )
SET ( EXAMPLE_SOURCE_DIR 	${EXAMPLE_DIR}/Castles/CastlesClient )

# global parameters
INCLUDE_DIRECTORIES(include)

# list here all example dirs
ADD_SUBDIRECTORY(src)

