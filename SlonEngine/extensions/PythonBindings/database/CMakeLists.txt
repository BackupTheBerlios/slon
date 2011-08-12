INCLUDE_DIRECTORIES (
    include
)

SET ( SOURCES
	stdafx.h
    stdafx.cpp
)

SET ( DATABASE_HEADERS
    include/PyDatabaseManager.h
	include/PyLibrary.h
)

SET ( DATABASE_SOURCES
	src/PyDatabaseManager.cpp
	src/PyLibrary.cpp
    src/Export.cpp
)

SOURCE_GROUP( "" FILES          ${SOURCES} )
SOURCE_GROUP( headers FILES     ${DATABASE_HEADERS} )
SOURCE_GROUP( sources FILES     ${DATABASE_SOURCES} )

ADD_LIBRARY( PyDatabase SHARED
    ${SOURCES}
    ${DATABASE_HEADERS} 
    ${DATABASE_SOURCES} 
)

ADD_PRECOMPILED_HEADER( PyDatabase "stdafx.h" "stdafx.cpp" )

SET_TARGET_PROPERTIES( PyDatabase PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}"
    FOLDER                   ${PYTHON_BINDINGS_PROJECT_GROUP}
    DEBUG_POSTFIX            ""
    OUTPUT_NAME              "database"
    PREFIX                   "extensions/PythonBindings/slon/"
    SUFFIX                   ".pyd"
)

TARGET_LINK_LIBRARIES( PyDatabase
    ${TARGET_UNIX_NAME}
)

