# vars
SET ( EXAMPLE_NAME			CastlesServer )
SET ( EXAMPLE_INSTALL_DIR	${EXAMPLE_INSTALL_DIR}/Castles/${EXAMPLE} )
SET ( EXAMPLE_SOURCE_DIR 	${EXAMPLE_DIR}/Castles/CastlesServer )

# global parameters
INCLUDE_DIRECTORIES(include)

# list here all example dirs
ADD_SUBDIRECTORY(src)

