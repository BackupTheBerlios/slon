INCLUDE_DIRECTORIES (
    include
)

SET ( SOURCES
	stdafx.h
    stdafx.cpp
)

SET ( SCENE_HEADERS
	include/PyGroup.h
	include/PyMatrixTransform.h
	include/PyNode.h
	include/PyTransform.h
)

SET ( SCENE_SOURCES
	src/PyGroup.cpp
	src/PyMatrixTransform.cpp
	src/PyNode.cpp
	src/PyTransform.cpp
    src/Export.cpp
)

SOURCE_GROUP( "" FILES          ${SOURCES} )
SOURCE_GROUP( headers FILES     ${SCENE_HEADERS} )
SOURCE_GROUP( sources FILES     ${SCENE_SOURCES} )

ADD_LIBRARY( PyScene SHARED
    ${SOURCES}
    ${SCENE_HEADERS} 
    ${SCENE_SOURCES} 
)

ADD_PRECOMPILED_HEADER( PyScene "${PYTHON_BINDINGS_DIR}/stdafx.h" "${PYTHON_BINDINGS_DIR}/stdafx.cpp" )

SET_TARGET_PROPERTIES( PyScene PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}"
    FOLDER                   ${PYTHON_BINDINGS_PROJECT_GROUP}
    DEBUG_POSTFIX            ""
    OUTPUT_NAME              "scene"
    PREFIX                   "extensions/PythonBindings/slon/"
    SUFFIX                   ".pyd"
)

TARGET_LINK_LIBRARIES( PyScene
    ${TARGET_UNIX_NAME}
)

