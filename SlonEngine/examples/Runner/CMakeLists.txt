# vars
SET ( EXAMPLE_NAME			Runner )
SET ( EXAMPLE_INSTALL_DIR	${EXAMPLE_INSTALL_DIR}/${EXAMPLE} )
SET ( EXAMPLE_SOURCE_DIR 	${EXAMPLE_DIR}/Runner )

# global parameters
INCLUDE_DIRECTORIES (
    include
)

# list here all example dirs
ADD_SUBDIRECTORY(src)

