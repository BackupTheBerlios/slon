INCLUDE_DIRECTORIES (
    include
)

SET ( SOURCES
	stdafx.h
    stdafx.cpp
)

SET ( GRAPHICS_HEADERS
	include/PyEffect.h
	include/PyGraphicsManager.h
	include/PyMesh.h
	include/PySGL.h
)

SET ( GRAPHICS_SOURCES
	src/PyEffect.cpp
	src/PyGraphicsManager.cpp
	src/PyMesh.cpp
	src/PySGL.cpp
    src/Export.cpp
)

SOURCE_GROUP( "" FILES          ${SOURCES} )
SOURCE_GROUP( headers FILES     ${GRAPHICS_HEADERS} )
SOURCE_GROUP( sources FILES     ${GRAPHICS_SOURCES} )

ADD_LIBRARY( PyGraphics SHARED
    ${SOURCES}
    ${GRAPHICS_HEADERS} 
    ${GRAPHICS_SOURCES} 
)

ADD_PRECOMPILED_HEADER( PyGraphics "${PYTHON_BINDINGS_DIR}/stdafx.h" "${PYTHON_BINDINGS_DIR}/stdafx.cpp" )

SET_TARGET_PROPERTIES( PyGraphics PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}"
    FOLDER                   ${PYTHON_BINDINGS_PROJECT_GROUP}
    DEBUG_POSTFIX            ""
    OUTPUT_NAME              "graphics"
    PREFIX                   "extensions/PythonBindings/slon/"
    SUFFIX                   ".pyd"
)

TARGET_LINK_LIBRARIES( PyGraphics
    ${TARGET_UNIX_NAME}
)

