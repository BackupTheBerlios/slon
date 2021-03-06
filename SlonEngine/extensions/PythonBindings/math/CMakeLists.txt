INCLUDE_DIRECTORIES (
    include
)

SET ( SOURCES
	../stdafx.h
    ../stdafx.cpp
)

SET ( MATH_HEADERS
	include/PyAABB.h
	include/PyPlane.h
	include/PyMatrix.h
	include/PyVector.h
)

SET ( MATH_SOURCES
	src/PyAABB.cpp
	src/PyPlane.cpp
	src/PyMatrix.cpp
	src/PyVector.cpp
    src/Export.cpp
)

SOURCE_GROUP( "" FILES          ${SOURCES} )
SOURCE_GROUP( headers FILES     ${MATH_HEADERS} )
SOURCE_GROUP( sources FILES     ${MATH_SOURCES} )

ADD_LIBRARY( PyMath SHARED
    ${SOURCES}
    ${MATH_HEADERS} 
    ${MATH_SOURCES} 
)

ADD_PRECOMPILED_HEADER( PyMath "${PYTHON_BINDINGS_DIR}/stdafx.h" "${PYTHON_BINDINGS_DIR}/stdafx.cpp" )

SET_TARGET_PROPERTIES( PyMath PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}"
    FOLDER                   ${PYTHON_BINDINGS_PROJECT_GROUP}
    DEBUG_POSTFIX            ""
    OUTPUT_NAME              "math"
    PREFIX                   "extensions/PythonBindings/slon/"
    SUFFIX                   ".pyd"
)

TARGET_LINK_LIBRARIES( PyMath
    ${TARGET_UNIX_NAME}
)

