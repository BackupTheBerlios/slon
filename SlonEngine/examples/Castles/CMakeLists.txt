SET ( EXAMPLE_NAME Castles )

# fin necessary libs
INCLUDE (FindNecessaryLibraries.cmake)

# global parameters
IF ( BUILD_${EXAMPLE_NAME} )
	INCLUDE_DIRECTORIES (include)

	ADD_SUBDIRECTORY (CastlesClient)
	ADD_SUBDIRECTORY (CastlesServer)
ENDIF ( BUILD_${EXAMPLE_NAME} )