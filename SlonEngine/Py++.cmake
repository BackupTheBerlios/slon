ADD_CUSTOM_TARGET (Py++)
SET_TARGET_PROPERTIES (Py++ PROPERTIES
	FOLDER Utility
)

SET (PYPP_OPTIONS
	-I ${PROJECT_SOURCE_DIR}/slon
	-I ${Boost_INCLUDE_DIR}
)

IF (MSVC)
    IF (MSVC90)
        LIST (APPEND PYPP_OPTIONS -C msvc9)
    ELSEIF (MSVC10)
        LIST (APPEND PYPP_OPTIONS -C msvc10)
    ENDIF (MSVC90)
ENDIF (MSVC)

GET_DIRECTORY_PROPERTY ( TARGET_DEFINITIONS COMPILE_DEFINITIONS )
MESSAGE (STATUS "GCCXML cxx flags: ${CMAKE_CXX_FLAGS}")

LIST (APPEND PYPP_OPTIONS "-F \"${CMAKE_CXX_FLAGS}\"")

ADD_CUSTOM_COMMAND( TARGET  Py++
				    COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/slon/Py++.py ${PYPP_OPTIONS} ${PROJECT_SOURCE_DIR} ${GCCXML} )
