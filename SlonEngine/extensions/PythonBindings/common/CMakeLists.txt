INCLUDE_DIRECTORIES (
    include
)

SET ( SOURCES
	stdafx.h
    stdafx.cpp
)

SET ( COMMON_HEADERS
	include/PyContainers.h
)

SET ( COMMON_SOURCES
	src/PyContainers.cpp
    src/Export.cpp
)

SOURCE_GROUP( "" FILES          ${SOURCES} )
SOURCE_GROUP( headers FILES     ${COMMON_HEADERS} )
SOURCE_GROUP( sources FILES     ${COMMON_SOURCES} )

ADD_LIBRARY( PyCommon SHARED
    ${SOURCES}
    ${COMMON_HEADERS} 
    ${COMMON_SOURCES} 
)

ADD_PRECOMPILED_HEADER( PyCommon "stdafx.h" "stdafx.cpp" )

SET_TARGET_PROPERTIES( PyCommon PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}"
    FOLDER                   ${PYTHON_BINDINGS_PROJECT_GROUP}
    DEBUG_POSTFIX            ""
    OUTPUT_NAME              "common"
    PREFIX                   "extensions/PythonBindings/slon/"
    SUFFIX                   ".pyd"
)

TARGET_LINK_LIBRARIES( PyCommon
    ${TARGET_UNIX_NAME}
)

