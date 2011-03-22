SET ( BUILD_${EXAMPLE_NAME} ON )

# boost
SET (Boost_ADDITIONAL_VERSIONS 1.36.*)
FIND_PACKAGE (Boost 1.36.0 COMPONENTS system thread)
IF (NOT Boost_FOUND)
	MESSAGE (STATUS "Project ${CMAKE_PROJECT_NAME} couldn't be completely build whithout boost system library")
	SET ( BUILD_${EXAMPLE_NAME} OFF )
ENDIF (NOT Boost_FOUND)

# bullet
INCLUDE (FindBullet)
IF (NOT BULLET_FOUND)
	MESSAGE (STATUS "Project ${CMAKE_PROJECT_NAME} couldn't be completely build whithout bullet library")
	SET ( BUILD_${EXAMPLE_NAME} OFF )
ENDIF (NOT BULLET_FOUND)

